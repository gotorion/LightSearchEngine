#include "RssReader.h"

#include "PageLibPreprocessor.h"
void RssReader::dump(PageLibPreprocessor& m)  //输出
{
    for (size_t i = 0; i < _rss.size(); i++) {
        m.insert(_rss[i]);
    }
}
void RssReader::parseRss(string& name)  //解析
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(name.c_str());
    tinyxml2::XMLNode* node = doc.FirstChildElement("rss")
                                  ->FirstChildElement("channel")
                                  ->FirstChildElement("item");
    traversingXML(node);
}

void RssReader::traversingXML(tinyxml2::XMLNode* node) {
    if (node == nullptr) return;
    tinyxml2::XMLElement* childTitle = node->FirstChildElement("title");
    tinyxml2::XMLElement* childLink = node->FirstChildElement("link");
    tinyxml2::XMLElement* childDescription =
        node->FirstChildElement("description");
    tinyxml2::XMLElement* childContent =
        node->FirstChildElement("content:encoded");
    RssItem m;
    if (childTitle != nullptr) {
        m.title = childTitle->GetText();
        replaceRegex(m.title);
    }
    if (childLink != nullptr) {
        m.link = childLink->GetText();
        replaceRegex(m.link);
    }
    if (childDescription != nullptr) {
        m.description = childDescription->GetText();
        replaceRegex(m.description);
    }
    if (childContent != nullptr) {
        m.content = childContent->GetText();
        replaceRegex(m.content);
    }
    if (childDescription != nullptr || childContent != nullptr) {
        _rss.push_back(m);
    }
    traversingXML(node->NextSibling());
}

void RssReader::replaceRegex(string& str) {
    // regex
    // regex1("<[^>]+>|'|^\u4e00-\u9fa5a-zA-Z0-9_|\\s|　|(&nbsp;)| |(showPlayer(.*);)");
    regex regex1("<[^>]+>");
    regex pattern("[!\"#$%&\'[\\]]+");
    string replacement = "";
    str = regex_replace(str, regex1, replacement);
    str = regex_replace(str, pattern, replacement);
}
