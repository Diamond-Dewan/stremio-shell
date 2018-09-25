FROM ubuntu:18.04
RUN apt update
RUN apt install -y nodejs build-essential qt5-default qtdeclarative5-dev qtdeclarative5-dev-tools qtwebengine5-dev qml-module-qtquick-controls qml-module-qtquick-dialogs qml-module-qt-labs-platform qml-module-qtwebchannel qml-module-qtwebengine wget libmpv-dev libssl-dev
WORKDIR /build-stremio
ADD . /build-stremio

RUN make -f AppImage.makefile clean
RUN make -f AppImage.makefile

