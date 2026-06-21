#pragma once
#include <ezlibs/ezClass.hpp>
#include <ezlibs/ezSingleton.hpp>
#include <ezlibs/ezXmlConfig.hpp>

class Backend : public ez::xml::Config {
    DISABLE_CONSTRUCTORS(Backend)
    DISABLE_DESTRUCTORS(Backend)
    IMPLEMENT_SINGLETON(Backend)

public:
    bool init();
    void unit();
    void startFrame();
    void endFrame();
    void justDropFiles(const std::vector<std::string>& aPaths);
    ez::xml::Nodes getXmlNodes(const std::string& aUserDatas) override;
    bool setFromXmlNodes(const ez::xml::Node& aNode, const ez::xml::Node& aParent, const std::string& aUserDatas) override;
};
