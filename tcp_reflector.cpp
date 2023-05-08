#include <Poco/Delegate.h>
#include <Poco/FIFOBuffer.h>
#include <Poco/Logger.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketAcceptor.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Util/ServerApplication.h>

const std::string BANNER = "Welcome to POCO TCP server. Enter you string:\n";
const uint16_t PORT = 28888;
const size_t MAX_MESSAGE = 256;
Poco::Logger &logger = Poco::Logger::get("");

struct ReflectorServiceHandler {
    template<class T>
    using Observer = Poco::Observer<ReflectorServiceHandler, T>;

    ReflectorServiceHandler(Poco::Net::StreamSocket &socket,
                            Poco::Net::SocketReactor &reactor)
        : socket(socket)
        , reactor(reactor)
        , read{*this, &ReflectorServiceHandler::handleRead}
        , write{*this, &ReflectorServiceHandler::handleWrite}
        , readbuf{MAX_MESSAGE, /*notify=*/ true}
        , writebuf{MAX_MESSAGE, /*notify=*/ true}
    {
        poco_information(logger, "+handler");
        writebuf.readable += Poco::delegate(this, &ReflectorServiceHandler::canWrite);
        readbuf.writable += Poco::delegate(this, &ReflectorServiceHandler::canRead);
        writebuf.write(BANNER.data(), BANNER.size());
        bool readable = true;
        canRead(readable);
    }

    ~ReflectorServiceHandler() {
        writebuf.readable -= Poco::delegate(this, &ReflectorServiceHandler::canWrite);
        readbuf.writable -= Poco::delegate(this, &ReflectorServiceHandler::canRead);
        poco_information(logger, "-handler");
    }

    void canWrite(bool& writeable) {
        if (writeable) {
            poco_information(logger, "+write");
            reactor.addEventHandler(socket, write);
        } else {
            poco_information(logger, "-write");
            reactor.removeEventHandler(socket, write);
            if (!reactor.hasEventHandler(socket, read))
                destroy = true;
        }
    }

    void canRead(bool& readable) {
        if (readable) {
            poco_information(logger, "+read");
            reactor.addEventHandler(socket, read);
        } else {
            poco_information(logger, "-read");
            reactor.removeEventHandler(socket, read);
            if (!reactor.hasEventHandler(socket, write))
                destroy = true;
        }
    }

    void handleRead(Poco::Net::ReadableNotification *event) {
        Poco::Net::StreamSocket socket{event->socket()};
        char *begin = readbuf.next();

        if (!socket.receiveBytes(readbuf)) {
            poco_information(logger, "eof");
            readbuf.setEOF();
        }

        newData(begin, readbuf.next());
        if (destroy)
            delete this;
    }

    void newData(char *begin, char *end) {
        poco_information_f(logger, "%d bytes read", (int)(end - begin));
        char *eol = std::find(begin, end, '\n');
        if (eol != end || readbuf.hasEOF())
            handleLine(eol);
        if (ready_bytes == 0 && readbuf.isFull())
            shutdown();
    }

    void handleLine(char *eol) {
        poco_information(logger, "line");
        char *line = readbuf.begin() + ready_bytes;
        std::reverse(line, eol);
        ready_bytes += eol + 1 - line;
        flush();
    }

    void flush() {
        size_t avail_bytes = std::min(ready_bytes, writebuf.available());
        if (!avail_bytes)
            return;
        writebuf.write(readbuf.begin(), avail_bytes);
        readbuf.drain(avail_bytes);
        ready_bytes -= avail_bytes;
    }

    void handleWrite(Poco::Net::WritableNotification *event) {
        Poco::Net::StreamSocket socket{event->socket()};
        if (socket.sendBytes(writebuf) > 0)
            flush();
        if (readbuf.hasEOF() && writebuf.isEmpty())
            shutdown();
        if (destroy)
            delete this;
    }

    void shutdown() {
        poco_information(logger, "shutdown");
        socket.shutdown();
    }

    Poco::Net::StreamSocket socket;
    Poco::Net::SocketReactor& reactor;
    Observer<Poco::Net::ReadableNotification> read;
    Observer<Poco::Net::WritableNotification> write;
    Poco::FIFOBuffer readbuf;
    Poco::FIFOBuffer writebuf;
    size_t ready_bytes = 0;
    bool destroy = false;
};

struct MyServerApplication : Poco::Util::ServerApplication {
  int main(const std::vector<std::string> &args)
  {
    Poco::Net::ServerSocket socket{PORT, /*backlog=*/64};
    Poco::Net::SocketReactor reactor;
    Poco::Net::SocketAcceptor<ReflectorServiceHandler> acceptor{socket};

    acceptor.setReactor(reactor);
    reactor.run();

    return Poco::Util::Application::EXIT_OK;
  }
};

int main(int argc, char** argv)
{
    MyServerApplication app;
    return app.run(argc, argv);
}
