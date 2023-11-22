import argparse
from json import dumps
from math import log2

import numpy
import pyopencl
from tqdm import tqdm


class MyFormatter(argparse.RawDescriptionHelpFormatter, argparse.ArgumentDefaultsHelpFormatter):
    pass


def get_args(argv=None) -> argparse.Namespace:
    platform_options = {
        'platforms': {
            i: {
                'name': platform.name,
                'version': platform.version,
                'devices': {j: device.name for j, device in enumerate(platform.get_devices())},
            }
            for i, platform in enumerate(pyopencl.get_platforms())
        }
    }
    parser = argparse.ArgumentParser(
        formatter_class=MyFormatter,
        epilog=(
            'devices and platforms:'
            + dumps(platform_options, indent=2).replace('{', '').replace('}', '')
        ),
    )

    parser.add_argument('--n', '-n', default=30, type=int, help='calculate primes up to 2^n')
    parser.add_argument('--platform', default=0, type=int, help='see epilog')
    parser.add_argument('--device', default=0, type=int, help='see epilog')
    parser.add_argument(
        '--segment',
        default=10,
        type=int,
        help='size of one segment of the segmented sieve is 2^segment',
    )
    parser.add_argument(
        '--local_size',
        default=256,
        type=int,
        help='this number of threads are grouped together on device',
    )
    parser.add_argument('--output', default='output.txt', type=str)
    args = parser.parse_args(argv)
    return args


def validate_arguments(args: argparse.Namespace) -> None:
    primes_upto_n = 2**args.n
    segment_size = 2**args.segment

    number_of_jobs = primes_upto_n // segment_size

    if number_of_jobs % args.local_size > 0:
        raise ValueError(
            f'local_size={args.local_size} must devide n/segment_size={number_of_jobs}'
        )


def main(args) -> None:
    validate_arguments(args)

    primes_upto_n = 2**args.nshift
    segment_size = 2**args.segment_size_shift

    platform = pyopencl.get_platforms()[args.platform]
    device = platform.get_devices()[args.device]

    # one thread computes `segment_size` primes
    # there are `n / segment_size` threads to compute
    # we do `batch_size` threads at one enqueue
    batch_size = min(
        primes_upto_n // segment_size,
        2 ** int(log2(pyopencl.kernel_work_group_info.WORK_GROUP_SIZE)),
    )

    # clear output file ahead
    with open(args.output, 'wb'):
        pass

    with pyopencl.CommandQueue(
        pyopencl.Context([device], properties=[(pyopencl.context_properties.PLATFORM, platform)])
    ) as queue:
        kernel = get_kernel_with_precomputed_primes(queue, primes_upto_n, segment_size)
        result, result_device = prepare_result(queue, batch_size, segment_size)
        kernel.set_arg(0, result_device)

        for offset in tqdm(range(0, primes_upto_n, batch_size * segment_size)):
            kernel.set_arg(2, numpy.uint64(offset))
            pyopencl.enqueue_nd_range_kernel(queue, kernel, (batch_size,), (args.local_size,))
            flush_results(queue, result, result_device, args.output)


def get_kernel_with_precomputed_primes(
    queue: pyopencl.CommandQueue, primes_upto_n: int, segment_size: int
) -> pyopencl.Kernel:
    context = queue.get_info(pyopencl.command_queue_info.CONTEXT)

    small_primes = calculate_small_primes(primes_upto_n)

    kernel = build_kernel(context, small_primes.size, segment_size)

    small_primes_device = pyopencl.Buffer(
        context,
        pyopencl.mem_flags.READ_ONLY + pyopencl.mem_flags.HOST_WRITE_ONLY,
        size=small_primes.nbytes,
    )
    pyopencl.enqueue_copy(queue, small_primes_device, small_primes.data)
    kernel.set_arg(1, small_primes_device)
    return kernel


def build_kernel(
    context: pyopencl.Context, small_primes_size: int, segment_size: int
) -> pyopencl.Kernel:
    segmented_sieve = pyopencl.Kernel(
        pyopencl.Program(
            context,
            f"""
            __kernel void segmented_sieve(
                __global uchar * result,
                __global ulong const * small_primes,
                ulong offset
            )
            {{
                size_t const segment_start = get_global_id(0) * {segment_size} + offset;
                size_t const segment_end = (get_global_id(0) + 1) * {segment_size} + offset;
                size_t start, i;
                ulong p;

                for (i = 0; i < {small_primes_size} ; ++i)
                {{
                    p = small_primes[i];
                    if (p*p > segment_end)
                        break;
                    for (
                        start = max((segment_start + p) / p, p) * p - 1;
                        start < segment_end;
                        start += p
                    )
                        result[start - offset] = {ord('0')};
                }}
            }}
            """,
        ).build(),
        'segmented_sieve',
    )
    return segmented_sieve


def calculate_small_primes(primes_upto_n: int) -> numpy.ndarray:
    # pylint: disable=no-member
    small_primes = (sieve(int(primes_upto_n**0.5)).nonzero()[0] + 1).astype('uint64')
    return small_primes


def sieve(primes_upto_n: int) -> numpy.ndarray:
    primes = numpy.ones(primes_upto_n, dtype=bool)
    i = 0
    primes[i] = False
    while i < primes_upto_n:
        i += 1
        while not primes[i]:
            i += 1
            if i == primes_upto_n:
                return primes
        prime = i + 1
        primes[prime * prime - 1 :: prime] = False
    return primes


def prepare_result(queue: pyopencl.CommandQueue, batch_size: int, segment_size: int):
    context = queue.get_info(pyopencl.command_queue_info.CONTEXT)

    result = numpy.zeros(batch_size * segment_size, dtype='ubyte')
    result_device = pyopencl.Buffer(
        context,
        pyopencl.mem_flags.WRITE_ONLY + pyopencl.mem_flags.HOST_READ_ONLY,
        size=result.nbytes,
    )

    pyopencl.enqueue_fill(queue, result_device, numpy.ubyte(ord('1')), result.size)
    # one is not a prime
    pyopencl.enqueue_fill(queue, result_device, numpy.ubyte(ord('0')), 1)

    return result, result_device


def flush_results(
    queue: pyopencl.CommandQueue,
    result: numpy.ndarray,
    result_device: pyopencl.Buffer,
    output_filename: str,
):
    pyopencl.enqueue_copy(queue, result.data, result_device)
    with open(output_filename, 'ab') as output_file:
        result.tofile(output_file)
    # reset for next batch
    pyopencl.enqueue_fill(queue, result_device, numpy.ubyte(ord('1')), result.size)


if __name__ == "__main__":
    main(get_args())
