#pragma once

/*
MIT License

Copyright (c) 2014-2024 Stephane Cuillerdier (aka aiekick)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// ezXml is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <map>
#include <stack>
#include <string>
#include <vector>
#include <cstdint>
#include <cassert>
#include <sstream>
#include <ostream>
#include <fstream>

namespace ez {
    class Xml;

    namespace xml {

        class Node;

        typedef std::vector<Node> Nodes;

        class Node {
            friend class ez::Xml;

        public:
            enum class Type {
                None = 0,
                Token,
                Comment
            };

            class Attribute {
            private:
                std::string m_value;
            public:
                Attribute() = default;

                explicit Attribute(const std::string &vValue) : m_value(vValue) {}

                template<typename T>
                Attribute &operator<<(const T &vValue) {
                    std::ostringstream vOut;
                    vOut << vValue;
                    m_value = vOut.str();
                    return *this;
                }

                friend std::ostream& operator<<(std::ostream& os, const Attribute& attr) {
                    os << attr.m_value;
                    return os;
                }

                const std::string& getValue() const {
                    return m_value;
                }
            };

        private:
            std::string m_name;
            std::map<std::string, Attribute> m_attributes;
            std::string m_content;
            std::string m_parentNodeName;
            Nodes m_children;
            Type m_type = Type::None;

        public:
            static std::string escapeXml(const std::string &vDatas) {
                std::string escaped = vDatas;
                replaceAll(escaped, "&", "&amp;");
                replaceAll(escaped, "<", "&lt;");
                replaceAll(escaped, "\"", "&quot;");
                replaceAll(escaped, "'", "&apos;");
                replaceAll(escaped, ">", "&gt;");
                return escaped;
            }

            // replace xml excaped pattern by corresponding good pattern
            static std::string unEscapeXml(const std::string &vDatas) {
                std::string unescaped = vDatas;
                replaceAll(unescaped, "&lt;", "<");
                replaceAll(unescaped, "&amp;", "&");
                replaceAll(unescaped, "&quot;", "\"");
                replaceAll(unescaped, "&apos;", "'");
                replaceAll(unescaped, "&gt;", ">");
                return unescaped;
            }

            static void replaceAll(std::string &vStr, const std::string &vFrom, const std::string &vTo) {
                if (vFrom.empty()) return;
                size_t startPos = 0;
                while ((startPos = vStr.find(vFrom, startPos)) != std::string::npos) {
                    vStr.replace(startPos, vFrom.length(), vTo);
                    startPos += vTo.length();
                }
            }

        public:
            Node(const std::string &vName = "") : m_name(vName) {
            }

            Node &setName(const std::string &vName) {
                m_name = vName;
                return *this;
            }

            Node &addChild(const Node &vChild) {
                m_children.push_back(vChild);
                m_children.back().m_setParentNodeName(getName());
                return m_children.back();
            }

            Node &addChild(const std::string &vName) {
                Node node(vName);
                return addChild(node);
            }

            Node &addComment(const std::string &vComment) {
                Node node;
                node.setContent(vComment).m_setType(Type::Comment);
                return addChild(node);
            }

            Node *getChild(const std::string &vName) {
                for (auto &child: m_children) {
                    if (child.m_name == vName) {
                        return &child;
                    }
                }
                return nullptr;
            }

            Node &getOrAddChild(const std::string &vName) {
                Node *ret = getChild(vName);
                if (ret == nullptr) {
                    ret = &addChild(vName);
                }
                return *ret;
            }

            Node &addChilds(const Nodes &vChilds) {
                for (const auto &node: vChilds) {
                    addChild(node);
                }
                return *this;
            }
            template<typename T>
            Node& addAttribute(const std::string& vKey, const T& vValue) {
                std::stringstream ss;
                ss << vValue;
                m_attributes[vKey] = Attribute(ss.str());
                return *this;
            }

            Attribute& addAttribute(const std::string& vKey) {
                m_attributes[vKey] = Attribute();
                return m_attributes[vKey];
            }

            bool isAttributeExist(const std::string &vKey) const {
                return (m_attributes.find(vKey) != m_attributes.end());
            }

            template<typename T = std::string>
            T getAttribute(const std::string &vKey) const {
                T ret;
                std::stringstream ss;
                auto it = m_attributes.find(vKey);
                if (it != m_attributes.end()) {
                    ss << it->second;
                }
                ss >> ret;
                return ret;
            }

            template<typename T>
            Node &setContent(const T &vContent) {
                std::stringstream ss;
                ss << vContent;
                m_content = ss.str();
                return *this;
            }

            Node &setContent(const std::string &vContent) {
                m_content = escapeXml(vContent);
                return *this;
            }

            // specific case for std::string
            template <typename T = std::string>
            typename std::enable_if<std::is_same<T, std::string>::value, T>::type getContent() const {
                return unEscapeXml(m_content);
            }

            // specific case for bool
            template <typename T>
            typename std::enable_if<std::is_same<T, bool>::value, T>::type getContent() const {
                return (m_content == "true");
            }

            // general cases (exclude std::string and bool)
            template <typename T>
            typename std::enable_if<!std::is_same<T, std::string>::value && !std::is_same<T, bool>::value, T>::type getContent() const {
                T ret;
                std::stringstream ss;
                ss << m_content;
                ss >> ret;
                return ret;
            }

            Nodes &getChildren() { return m_children; }

            const Nodes &getChildren() const { return m_children; }

            const std::string &getParentNodeName() const {
                return m_parentNodeName;
            }

            const std::string &getName() const {
                return m_name;
            }

            std::string dump(const xml::Node &vNode, const uint32_t vLevel = 0) const {
                std::string indent(vLevel * 2, ' ');  // Indentation based on the depth level
                std::ostringstream oss;

                oss << indent;
                if (vNode.m_getType() != xml::Node::Type::Comment) {
                    oss << "<" << vNode.getName();
                    for (const auto &attr: vNode.m_attributes) {
                        oss << " " << attr.first << "=\"" << xml::Node::escapeXml(attr.second.getValue()) << "\"";
                    }
                } else {
                    oss << "<!-- ";
                }

                const auto &content = vNode.getContent();
                const auto &children = vNode.getChildren();

                if (content.empty() && children.empty()) {
                    oss << "/>" << std::endl;
                } else {
                    if (vNode.m_getType() != xml::Node::Type::Comment) {
                        oss << ">";
                    }
                    if (!content.empty()) {
                        oss << xml::Node::escapeXml(content);
                    }
                    if (vNode.m_getType() == xml::Node::Type::Comment) {
                        oss << " -->" << std::endl;
                    }
                    if (!children.empty()) {
                        oss << std::endl;
                        for (const auto &child: children) {
                            oss << dump(child, vLevel + 1);
                        }
                        oss << indent;
                    }
                    if (vNode.m_getType() != xml::Node::Type::Comment) {
                        oss << "</" << vNode.getName() << ">" << std::endl;
                    }
                }

                return oss.str();
            }

            std::string dump() const {
                return dump(*this);
            }

        private:

            void m_setAttribute(const std::string &vKey, const std::string &vValue) {
                m_attributes[vKey] = Attribute(vValue);
            }

            void m_setType(Type vType) {
                m_type = vType;
            }

            Type m_getType() const {
                return m_type;
            }

            void m_setParentNodeName(const std::string &vName) {
                m_parentNodeName = vName;
            }
        };

        template <>
        inline Node &Node::setContent(const bool &vContent) {
            m_content = vContent ? "true" : "false";
            return *this;
        }

#if __cplusplus >= 201703L  // c++17
        template <>
        inline bool Node::getContent<bool>() const {
            return (m_content == "true");
        }
#endif

    }  // namespace xml

    class Xml {
    private:
        xml::Node m_Root;
        // just during parsing,
        // for know what is the current node
        std::stack<xml::Node *> m_NodeStack;
        enum class TokenType {  //
            OPENED = 0,
            CLOSED,
            OPENED_CLOSED,
            COMMENT,
            CONTENT,
            Count
        };

    public:
        Xml(const std::string &vRootName = "root") : m_Root(vRootName) {
            m_NodeStack.push(&m_Root);
        }

        xml::Node &getRoot() {
            return m_Root;
        }

        bool parseFile(const std::string &vFilePathName) {
            std::ifstream docFile(vFilePathName, std::ios::in);
            if (docFile.is_open()) {
                std::stringstream strStream;
                strStream << docFile.rdbuf();  // read the file
                auto xml_content = strStream.str();
                m_replaceString(xml_content, "\r\n", "\n");
                m_replaceString(xml_content, "\r", "\n");
                docFile.close();
                return parseString(xml_content);
            }
            return false;
        }

        bool parseString(const std::string &vDoc) {
            auto tokens = m_tokenize(vDoc);
            if (tokens.empty()) {
                return false;
            }
            for (const auto &token: tokens) {
                std::string tagName;
                std::map<std::string, std::string> attributes;
                if (token.first.empty()) {
                    continue;
                }
                if (token.second == TokenType::CLOSED) {
                    m_NodeStack.pop();
                } else if (token.second == TokenType::OPENED ||         //
                           token.second == TokenType::OPENED_CLOSED ||  //
                           token.second == TokenType::COMMENT) {
                    if (token.second != TokenType::COMMENT) {
                        tagName = m_extractTagName(token.first);
                    }
                    xml::Node newNode(tagName);
                    newNode.m_setType(xml::Node::Type::Token);
                    if (token.second == TokenType::COMMENT) {
                        newNode.m_setType(xml::Node::Type::Comment);
                        newNode.setContent(token.first);
                    } else {
                        if (!m_extractAttributes(tagName, token.first, attributes)) {
                            return false;
                        }
                        for (const auto &kv: attributes) {
                            newNode.m_setAttribute(kv.first, kv.second);
                        }
                    }
                    m_NodeStack.top()->addChild(newNode);
                    if (token.second == TokenType::OPENED) {
                        m_NodeStack.push(const_cast<xml::Node *>(&m_NodeStack.top()->getChildren().back()));
                    }
                } else if (token.second == TokenType::CONTENT) {
                    if (!m_NodeStack.empty()) {
                        if (token.first[0] != '\n') {
                            m_NodeStack.top()->setContent(token.first);
                        }
                    }
                }
            }
            return true;
        }

        std::string dump() const {
            return m_Root.dump();
        }

    private:
        bool m_replaceString(std::string &str, const std::string &oldStr, const std::string &newStr) {
            bool found = false;
            size_t pos = 0;
            while ((pos = str.find(oldStr, pos)) != std::string::npos) {
                found = true;
                str.replace(pos, oldStr.length(), newStr);
                pos += newStr.length();
            }
            return found;
        }

        std::vector<std::pair<std::string, TokenType>> m_tokenize(const std::string &vDoc) {
            std::vector<std::pair<std::string, TokenType>> tokens;
            size_t pos = 0;
            size_t length = vDoc.length();
            TokenType type = TokenType::Count;
            while (pos < length) {
                if (vDoc[pos] == '<') {
                    type = TokenType::OPENED;
                    if (vDoc[pos + 1] == '/') {
                        type = TokenType::CLOSED;
                    } else if (vDoc[pos + 1] == '!') {
                        type = TokenType::COMMENT;
                    }
                    size_t end = vDoc.find(">", pos);
                    if (end == std::string::npos) {
                        break;
                    }
                    if (vDoc[end - 1] == '/') {
                        type = TokenType::OPENED_CLOSED;
                    }
                    const auto ss = vDoc.substr(pos, end + 1 - pos);
                    tokens.push_back(std::make_pair(ss, type));
                    pos = end + 1;
                } else {
                    size_t end = vDoc.find('<', pos);
                    const auto ss = vDoc.substr(pos, end - pos);
                    tokens.push_back(std::make_pair(ss, TokenType::CONTENT));
                    pos = end;
                }
            }
            return tokens;
        }

        std::string m_extractTagName(const std::string &vLine) {
            std::string ret;
            size_t startPos = vLine.find('<') + 1;
            while (vLine.at(startPos) == ' ' && vLine.size() > startPos) {
                ++startPos;
            }
            size_t endPos = vLine.find_first_of(" \t/>", startPos);
            return vLine.substr(startPos, endPos - startPos);
        }

        // will remove space outside of <> and ""
        std::string m_trim1(const std::string &vToken) {
            std::string res;
            int32_t scope = 0;
            for (const auto c: vToken) {
                if (scope == 1) {
                    res += c;
                } else if (c == '"') {
                    if (scope == 0) {
                        ++scope;
                    } else {
                        --scope;
                    }
                }
            }
            return res;
        }

        // will remove spaces from start and from end
        // if the token is like tk0 tk1 tk2, only tk0 will be returned
        std::string m_trim2(const std::string &vToken) {
            std::string res;
            for (const auto c: vToken) {
                if (c != ' ') {
                    res += c;
                } else if (!res.empty()) {
                    break;
                }
            }
            return res;
        }

        bool m_extractAttributes(const std::string &vTagName, const std::string &vLine,
                                 std::map<std::string, std::string> &attributes) {
            size_t startPos = vLine.find(vTagName);
            if (startPos != std::string::npos) {
                startPos += vTagName.size();
                startPos = vLine.find(' ', startPos);
                while (startPos != std::string::npos) {
                    startPos = vLine.find_first_not_of(" \t", startPos);
                    if (vLine.at(startPos) == '>') {
                        break;
                    }
                    size_t equalsPos = vLine.find('=', startPos);
                    if (equalsPos == std::string::npos) {
                        return false;
                    }
                    std::string key = m_trim2(vLine.substr(startPos, equalsPos - startPos));
                    startPos = equalsPos + 1;
                    char quoteChar = vLine[startPos];
                    while (quoteChar == ' ') {
                        quoteChar = vLine[++startPos];
                    }
                    if (quoteChar == '"' || quoteChar == '\'') {
                        startPos++;
                        size_t endPos = vLine.find(quoteChar, startPos);
                        if (endPos != std::string::npos) {
                            std::string value = vLine.substr(startPos, endPos - startPos);
                            attributes[key] = value;
                            startPos = vLine.find(' ', endPos);  // Passer ? l'attribut suivant
                        } else {
#ifdef LogVarError
                            LogVarError("The attribut '%s' have invalid value", key.c_str());
#endif
                            return false;  // Erreur : guillemets/apostrophes non ferm?s
                        }
                    } else {
#ifdef LogVarError
                        LogVarError("The attribut '%s' have invalid value", key.c_str());
#endif
                        return false;  // Erreur : attribut sans guillemets ou apostrophes
                    }
                }
                return true;
            }
            return false;
        }
    };

}  // namespace ez
