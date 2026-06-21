#include "backend.h"

bool Backend::init() {
    return true;
}

void Backend::unit() {
}

void Backend::startFrame() {
}

void Backend::endFrame() {

}

void Backend::justDropFiles(const std::vector<std::string>& aPaths) {
    // load the frist drop cdp file
}

ez::xml::Nodes Backend::getXmlNodes(const std::string& aUserDatas) {
    ez::xml::Node node;
    return node.getChildren();
}

bool Backend::setFromXmlNodes(const ez::xml::Node& aNode, const ez::xml::Node& aParent, const std::string& aUserDatas) {
    return true; // continue explore childrens
}
