#ifndef SCRIPTENGINE_HPP
#define SCRIPTENGINE_HPP

#include <string>
#include <vector>
#include <memory>

class ScriptEngine {
public:
    enum class Speaker {
        PLAYER,
        NPC,
        SYSTEM
    };

    enum class CommandType {
        NONE,
        ITEM,
        HIDE,
        SHOP,
        NOTICE,
        END
    };

    struct ScriptStep {
        Speaker speaker = Speaker::SYSTEM;
        std::string text;    // Merged dialogue text
        CommandType command = CommandType::NONE;
        std::string extra;   // Command parameters
    };

    ScriptEngine() = default;
    ~ScriptEngine() = default;

    /**
     * @brief Load and pre-process a script from a CSV file.
     * Groups consecutive dialogue lines into steps (max 3 lines).
     */
    void LoadScript(const std::string& name);

    /**
     * @brief Replace text placeholders in the loaded steps.
     */
    void ReplaceText(const std::string& target, const std::string& replacement);

    bool HasNext() const;
    ScriptStep Next();
    const ScriptStep& Peek() const;
    
    void Reset();
    void Clear();

    /**
     * @brief Inject a step (used for shop injection).
     */
    void InjectStep(const ScriptStep& step);

    /**
     * @brief For modifying the last step (e.g. shop injection into end command).
     */
    std::vector<ScriptStep>& GetSteps() { return m_steps; }

    size_t GetCurrentIndex() const { return m_currentIndex; }
    void SetCurrentIndex(size_t index) { m_currentIndex = index; }
    size_t GetSize() const { return m_steps.size(); }

private:
    std::vector<ScriptStep> m_steps;
    size_t m_currentIndex = 0;
};

#endif // SCRIPTENGINE_HPP
