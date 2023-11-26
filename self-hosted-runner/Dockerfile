FROM mcr.microsoft.com/windows:ltsc2019

ARG GITHUB_REPO
ARG GITHUB_TOKEN
ARG LABELS=

RUN mkdir actions-runner
WORKDIR actions-runner

ADD https://github.com/actions/runner/releases/download/v2.311.0/actions-runner-win-x64-2.311.0.zip .
RUN powershell.exe -NoP -NonI -Command "Expand-Archive actions-runner-win-x64-2.311.0.zip ."
RUN del actions-runner-win-x64-2.311.0.zip
RUN powershell.exe -NoP -NonI -Command "Set-ExecutionPolicy -ExecutionPolicy RemoteSigned"
SHELL ["cmd", "/S", "/C"]
RUN config.cmd --unattended --labels %LABELS% --url %GITHUB_REPO% --token %GITHUB_TOKEN%

ENTRYPOINT ["run.cmd"]
