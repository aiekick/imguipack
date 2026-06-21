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

// ezActions is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <list>
#include <functional>

namespace ez {

class Actions {
private:
    typedef std::function<bool()> ConditonalActionStamp;
    typedef std::function<void()> ImmediateActionStamp;
    std::list<ConditonalActionStamp> m_conditionalActions;
    std::list<ImmediateActionStamp> m_immediateActions;

public:
    // insert an action at first, cause :
    // this action will be executed first at the next run
    void pushFrontConditonalAction(ConditonalActionStamp vAction) {
        if (vAction) {
            m_conditionalActions.push_front(vAction);
        }
    }

    void pushFrontImmediateAction(ImmediateActionStamp vAction) {
        if (vAction) {
            m_immediateActions.push_front(vAction);
        }
    }

    // add an action at end
    // this action will be executed at least after all others
    void pushBackConditonalAction(ConditonalActionStamp vAction) {
        if (vAction) {
            m_conditionalActions.push_back(vAction);
        }
    }
    void pushBackImmediateAction(ImmediateActionStamp vAction) {
        if (vAction) {
            m_immediateActions.push_back(vAction);
        }
    }

    // clear all actions
    void clear() {
        m_conditionalActions.clear();
        m_immediateActions.clear();
    }

    // usefull for immediate mode gui actions (imgui or similar)
    // execute the first conditional action until true is returned
    // if return true, erase action let the next execution call the next action
    // il false, the action is executed until true
    void executeFirstConditionalAction() {
        if (!m_conditionalActions.empty()) {
            const auto action = *m_conditionalActions.begin();
            if (action()) {  // one action per run, it true we can continue by deleting the current
                if (!m_conditionalActions.empty()) {  // because an action can clear actions
                    m_conditionalActions.pop_front();
                }
            }
        }
    }

    // standard pre registered action
    // will execute all action in the list from front to back
    // then will clear the list
    void runImmediateActions() {
        while (!m_immediateActions.empty()) {
            const auto action = *m_immediateActions.begin();
            action();
            if (!m_immediateActions.empty()) {  // because an action can clear actions
                m_immediateActions.pop_front();
            }
        }
    }
};

}  // namespace ez
