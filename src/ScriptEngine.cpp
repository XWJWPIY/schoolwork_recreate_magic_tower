#include "ScriptEngine.hpp"
#include "AppUtil.hpp"
#include "Util/Logger.hpp"

void ScriptEngine::LoadScript(const std::string& name) {
    m_steps.clear();
    m_currentIndex = 0;

    std::string path = AppUtil::GetStaticResourcePath("Datas/Texts/" + name + ".csv");
    auto rows = AppUtil::MapParser::ParseCsvToStrings(path);

    bool skipHeader = true;
    for (const auto& row : rows) {
        if (row.empty()) continue;
        if (skipHeader) { skipHeader = false; continue; }
        
        std::string first = row[0];
        
        if (first == ".") {
            m_steps.push_back({Speaker::SYSTEM, "", CommandType::END, ""});
            break;
        }
        
        if (first == "0" || first == "1") {
            Speaker currentSpeaker = (first == "0") ? Speaker::PLAYER : Speaker::NPC;
            std::string text = (row.size() > 1) ? row[1] : "";

            // Check if we can merge with the previous step
            if (!m_steps.empty() && 
                m_steps.back().command == CommandType::NONE && 
                m_steps.back().speaker == currentSpeaker) {
                
                // Count existing lines (split by \n)
                int lineCount = 1;
                for (char c : m_steps.back().text) if (c == '\n') lineCount++;

                if (lineCount < 3) {
                    m_steps.back().text += "\n" + text;
                    continue;
                }
            }
            
            // Otherwise, create a new speech step
            m_steps.push_back({currentSpeaker, text, CommandType::NONE, ""});
        } 
        else if (first == "item") {
            if (row.size() >= 3) {
                m_steps.push_back({Speaker::SYSTEM, row[1], CommandType::ITEM, row[2]});
            }
        } 
        else if (first == "hide") {
            m_steps.push_back({Speaker::SYSTEM, "", CommandType::HIDE, ""});
        } 
        else if (first == "shop") {
            m_steps.push_back({Speaker::SYSTEM, "", CommandType::SHOP, ""});
        } 
        else {
            // Fallback for custom or old labels
            m_steps.push_back({Speaker::NPC, row.size() > 1 ? row[1] : "", CommandType::NONE, ""});
        }
    }
}

void ScriptEngine::ReplaceText(const std::string& target, const std::string& replacement) {
    for (auto& step : m_steps) {
        size_t pos = step.text.find(target);
        while (pos != std::string::npos) {
            step.text.replace(pos, target.length(), replacement);
            pos = step.text.find(target, pos + replacement.length());
        }
    }
}

bool ScriptEngine::HasNext() const {
    return m_currentIndex < m_steps.size();
}

ScriptEngine::ScriptStep ScriptEngine::Next() {
    if (!HasNext()) return {Speaker::SYSTEM, "", CommandType::END, ""};
    return m_steps[m_currentIndex++];
}

const ScriptEngine::ScriptStep& ScriptEngine::Peek() const {
    return m_steps.at(m_currentIndex);
}

void ScriptEngine::Reset() {
    m_currentIndex = 0;
}

void ScriptEngine::Clear() {
    m_steps.clear();
    m_currentIndex = 0;
}

void ScriptEngine::InjectStep(const ScriptStep& step) {
    m_steps.push_back(step);
}
