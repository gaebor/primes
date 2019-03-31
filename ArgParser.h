#pragma once

#include <typeinfo>
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include <iostream>
#include <sstream>

namespace arg
{
    template<class Str, class Ty>
    bool ReadVal(Ty& val, const Str& arg)
    {
        std::istringstream iss;
        iss.str(arg);
        iss >> val;
        return !iss.fail();
    }

    struct Argument
    {
        Argument(const std::initializer_list<const char*>& args, const std::string& info)
            : options(args.begin(), args.end()), _info(info)
        {
        }
        ~Argument() {}
        //! writes the corresponding help
        virtual void Write(std::ostream& os)const = 0;
        virtual void WriteShort(std::ostream& os)const = 0;
        //! returns the number of arguments consumed. If 0 then the read was not successful.
        virtual int Read(int argc, const char** argv)const = 0;

        const std::vector<std::string> options;
        const std::string _info;
    protected:
        bool Match(const char* arg) const
        {
            if (options.empty())
                return true;
            for (const auto& option : options)
                if (option == arg)
                    return true;
            return false;
        }
    };

    template<class Ty>
    struct TypedArgument : Argument
    {
        TypedArgument(Ty& def_val, 
            const std::initializer_list<const char*>& args = {},
            const std::string& info = "", const std::string& meta = "",
            const std::initializer_list<Ty>& choices = {})
            :   Argument(args, info), _val(def_val),
                _meta(meta.empty() ? typeid(_val).name() : meta),
                _choices(choices.begin(), choices.end())
        {
            if (!_choices.empty() && _choices.find(_val) == _choices.end())
            {
                std::cerr << "At option";
                for (auto o : options)
                    std::cerr << " " << o;
                std::cerr << " the default value \"" << _val << "\" is not listed in the choices!" << std::endl;
                exit(1);
            }
        }
        ~TypedArgument() {}
        virtual void WriteShort(std::ostream& os)const
        {
            os << " [" << options[0] << " '" <<  _meta << "']";
        }
        //! writes the corresponding help
        virtual void Write(std::ostream& os)const
        {
            os << "\t";
            for (auto option : options)
                os << option << " ";
            os << "'" << _meta << "' default: " << _val;
            
            if (!_info.empty())
                os << "\n\t\t" << _info;
            
            if (!_choices.empty())
            {
                os << "\n\t\tpossible values:";
                for (const auto& choice : _choices)
                    os << ' ' << choice;
            }
            os << std::endl;
        }
        //! returns the number of arguments consumed. If 0 then the read was not successful.
        virtual int Read(int argc, const char** argv)const
        {
            if (argc > 0)
            {
                if (options.empty())
                {
                    return ReadVal(_val, argv[0]) ? 1 : 0;
                }else if (Match(argv[0]) && argc > 1)
                {
                    if (ReadVal(_val, argv[1]) && (_choices.empty() || _choices.find(_val) != _choices.end()))
                        return 2;
                    std::cerr << "At option \"" << argv[0] << "\" the argument \"" << argv[1] << "\" is not valid!" << std::endl;
                    exit(1);
                }
            }
            return 0;
        }
    protected:
        Ty & _val;
        const std::string _meta;
        std::unordered_set<Ty> _choices;
    };

    struct SetFlag : Argument
    {
        SetFlag(bool& def_val,
            const std::initializer_list<const char*>& args = {},
            const std::string& info = "", bool set_to=true, const std::string& meta = "")
            : Argument(args, info), _val(def_val), _set_to(set_to),
            _meta(meta.empty() ? typeid(_val).name() : meta)
        {
        }
        ~SetFlag() {}
        virtual void WriteShort(std::ostream& os)const
        {
            os << " [" << options[0] << "]";
        }
        //! writes the corresponding help
        virtual void Write(std::ostream& os)const
        {
            os << "\t";
            for (auto option : options)
                os << option << " ";
            os << "'" << _meta << "' default: " << (_val ? "true" : "false");
            if (!_info.empty())
                os << "\n\t\t" << _info;

            os << std::endl;
        }
        //! returns the number of arguments consumed. If 0 then the read was not successful.
        virtual int Read(int argc, const char** argv)const
        {
            if (argc > 0)
            {
                if (Match(argv[0]))
                {
                    _val = _set_to;
                    return 1;
                }
            }
            return 0;
        }
    protected:
        bool& _val;
        bool _set_to;
        const std::string _meta;
    };

    class Parser
    {
    public:
        Parser(const std::string& info,
                const std::initializer_list<const char*>& helps = { "-h", "--help" })
            :   arguments(), _options(), program_name(),
                header(info), help_options(helps.begin(), helps.end())
        {
        }
        ~Parser() {}

        void Do(int argc, const char** argv)
        {
            program_name = argv[0];
            for (++argv, --argc; argc > 0;)
            {
                for (const auto& help : help_options)
                {
                    if (help == *argv)
                    {
                        Help(std::cout);
                        exit(0);
                    }
                }
                bool found = false;
                for (const auto& argument : arguments)
                {
                    const auto read = argument->Read(argc, argv);
                    if (read > 0)
                    {
                        argc -= read;
                        argv += read;
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    std::cerr << "Unknown argument: \"" << *argv << "\"" << std::endl;
                    ++argv;
                    --argc;
                }
            }
        }

        void Help(std::ostream& os = std::cout) const
        {
            if (!header.empty())
                os << header << "\n\n";
            os << "USAGE:\n\t" << program_name;
            for (const auto& argument : arguments)
                argument->WriteShort(os);
            os << "\n";
            os << "OPTIONS:\n";
            for (const auto& argument : arguments)
                argument->Write(os);
        }
        template <class Ty>
        void AddArg(Ty& value,
            const std::initializer_list<const char*>& args = {},
            const std::string& info = "", const std::string& meta = "",
            const std::initializer_list<Ty>& choices = {})
        {
            arguments.emplace_back(new TypedArgument<Ty>(value, args, info, meta, choices));

            for (const auto& option : arguments.back()->options)
                if (!_options.insert(option).second)
                {
                    std::cerr << "Duplicate option: \"" << option  << "\"" << std::endl;
                    exit(1);
                }
        }
        void AddFlag(bool& value,
            const std::initializer_list<const char*>& args = {},
            const std::string& info = "", bool set_to=true, const std::string& meta = "")
        {
            arguments.emplace_back(new SetFlag(value, args, info, set_to, meta));

            for (const auto& option : arguments.back()->options)
                if (!_options.insert(option).second)
                {
                    std::cerr << "Duplicate option: \"" << option << "\"" << std::endl;
                    exit(1);
                }
        }

    private:
        std::vector<std::unique_ptr<Argument>> arguments;
        std::vector<std::string> help_options;
        std::unordered_set<std::string> _options;
        std::string program_name;
        std::string header;
    };
}
