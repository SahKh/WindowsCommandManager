#ifndef SYSTEM_COMMAND_HANDLER_H
#define SYSTEM_COMMAND_HANDLER_H

#include <memory> 
#include <string>

class SystemCommandHandler {
public:
    virtual ~SystemCommandHandler() = default;

    virtual bool handleCommand(const std::string& command) = 0;

    virtual std::string getName() const = 0;
};

typedef std::shared_ptr<SystemCommandHandler> SystemCommandHandlerPtr;

//Control Classes
class VolumeControl : public SystemCommandHandler {
private:
    void adjustVolume(int newVolumeLevel);
public:
    bool handleCommand(const std::string& command) override;
    std::string getName() const override;
};

class MediaControl : public SystemCommandHandler {
private:
    void play_pause();
    void skipForward();
    void skipBackward();
public:
    bool handleCommand(const std::string& command) override;
    std::string getName() const override;
};

class ShutdownProtocol : public SystemCommandHandler {
private:
    bool shutdownInit();
public:
    bool handleCommand(const std::string& command) override;
    std::string getName() const override;
};

class SleepProtocol : public SystemCommandHandler {
private:
    bool sleepInit();
public:
    bool handleCommand(const std::string& command) override;
    std::string getName() const override;
};

class RestartProtocol : public SystemCommandHandler {
private:
    bool restartInit();
public:
    bool handleCommand(const std::string& command) override;
    std::string getName() const override;
};

class SystemCommandHandler_initializer : public SystemCommandHandler {
public:
    virtual SystemCommandHandlerPtr createCommandHandler(const std::string& commandType) const = 0;
};

#endif
