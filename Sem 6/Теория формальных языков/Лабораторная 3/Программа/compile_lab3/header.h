#pragma once

#include <vector>
#include <ranges>
#include <algorithm>
#include <deque>
#include "action.h"
#include <iostream>
#include <functional>
#include <unordered_set>

#include <random>


enum class FSMResult {
    Match,
    Mismatch,
    OutOfAlphabet
};


template<typename Data, typename ActionOut>
    requires std::ranges::range<Data>
class FSM {
private:

    int currentState;

    using ElementType = std::ranges::range_value_t<Data>;
    using Condition = std::function<bool(ElementType)>;
    using ActionPtr = Action<ActionOut(ElementType)>*;
    using Harvester = std::function<void(ActionOut)>;

    ElementType element;

    std::deque<ElementType> dataProc;

    std::vector<ActionPtr> matrixActions;
    std::vector<Condition> matrixConditions;
    std::vector<int> matrixStates;

    std::vector<int> offsets;
    std::vector<int> finalStates;

    std::function<void(ActionOut)>outputsHarvester;

    std::vector<std::vector<ElementType>> elemsConditions;

    bool flagEnd = false;

    bool doStep() {
        int condStart = offsets[currentState];
        int condEnd = offsets[currentState + 1];
        
        if (dataProc.size() > 0) {
            element = dataProc.front();
            dataProc.pop_front();
        }
        else {
            if (std::count(finalStates.begin(), finalStates.end(), currentState) > 0) {
                flagEnd = true;
                return false;
            }
        }

        for (int i = condStart; i < condEnd; i++) {
            if (matrixConditions[i](element)) {
                if (matrixActions[i]) {
                    ActionOut res = (*matrixActions[i])(element);
                    if (outputsHarvester) {
                        outputsHarvester(res);
                    }
                }
                currentState = matrixStates[i];

                return true;
            }
        }
        return false;
    };


    // при слиянии
    FSM(
        int stateCount,
        std::unordered_map<int, std::vector<std::tuple<int, Condition, ActionPtr>>> transitions,
        std::vector<int> finals,
        std::function<void(ActionOut)> harvester
    ) : outputsHarvester(harvester)
    {
        finalStates = std::move(finals);
        offsets.resize(stateCount + 1);

        for (int state = 0; state < stateCount; ++state) {
            offsets[state] = static_cast<int>(matrixActions.size());

            auto it = transitions.find(state);
            if (it != transitions.end()) {
                for (const auto& [to, cond, act] : it->second) {
                    if (cond) {
                        matrixActions.push_back(act);
                        matrixConditions.push_back(cond);
                        matrixStates.push_back(to);
                    }
                }
            }
        }
        offsets[stateCount] = static_cast<int>(matrixActions.size());
    }

public:
    FSM() = default;

    FSM(
        std::vector<std::vector<Condition>> conditions,
        std::vector<std::vector<ActionPtr>> actions,
        std::vector<int> finals,
        std::function<void(ActionOut)> harvester = nullptr
    ) : outputsHarvester(harvester)
    {
        if (actions.size() == conditions.size()) {
            finalStates = finals;
            int stateCount = actions.size();
            offsets.reserve(stateCount + 1);
            offsets.push_back(0);

            for (int i = 0; i < stateCount; i++) {
                for (int j = 0; j < stateCount; j++) {
                    if (conditions[i][j]) {
                        matrixActions.push_back(actions[i][j]);
                        matrixConditions.push_back(conditions[i][j]);
                        matrixStates.push_back(j);
                    }
                }
                offsets.push_back(matrixActions.size());
            }
        }
    }

    using Transition = std::pair<int, std::pair<Condition, ActionPtr>>;
    using TransitionMap = std::unordered_map<int, std::vector<Transition>>;

    FSM(
        int stateCount,
        TransitionMap transitions,
        std::vector<int> finals,
        std::function<void(ActionOut)> harvester = nullptr
    ) : outputsHarvester(harvester)
    {
        finalStates = std::move(finals);
        offsets.resize(stateCount + 1);

        for (int state = 0; state < stateCount; ++state) {
            offsets[state] = static_cast<int>(matrixActions.size());
            
            auto it = transitions.find(state);
            if (it != transitions.end()) {
                for (const auto& [to, condAct] : it->second) {
                    auto [condition, action] = condAct;
                    if (condition) {
                        matrixActions.push_back(action);
                        matrixConditions.push_back(condition);
                        matrixStates.push_back(to);
                    }
                }
            }
        }
        offsets[stateCount] = static_cast<int>(matrixActions.size());
    }

    using TransitionNull = std::pair<int, Condition>;
    using TransitionMapNull = std::unordered_map<int, std::vector<TransitionNull>>;

    FSM(
        int stateCount,
        TransitionMapNull transitions,
        std::vector<int> finals,
        ActionPtr allAction = nullptr,
        std::function<void(ActionOut)> harvester = nullptr
    ) : outputsHarvester(harvester)
    {
        finalStates = std::move(finals);
        offsets.resize(stateCount + 1);

        for (int state = 0; state < stateCount; ++state) {
            offsets[state] = static_cast<int>(matrixActions.size());

            auto it = transitions.find(state);
            if (it != transitions.end()) {
                for (const auto& [to, cond] : it->second) {
                    auto condition = cond;
                    if (condition) {
                        matrixActions.push_back(allAction);
                        matrixConditions.push_back(condition);
                        matrixStates.push_back(to);
                    }
                }
            }
        }
        offsets[stateCount] = static_cast<int>(matrixActions.size());
    }


    FSMResult start(Data& data, int startState = 0, Data* alphabet = nullptr) {
        if (data.empty()) {
            return FSMResult::Mismatch;
        }
        if (startState < offsets.size() - 1) {
            dataProc.clear();
            for (const ElementType& element : data) {
                dataProc.push_back(element);
            }
            currentState = startState;
        }
        flagEnd = false;
        while (doStep()) { }
        if (flagEnd) {
            return FSMResult::Match;
        }
        if (alphabet) {
            for (ElementType alphElem : *alphabet) {
                if (alphElem == element) {
                    return FSMResult::Mismatch;
                }
            }
            return FSMResult::OutOfAlphabet;
        }
        return FSMResult::Mismatch;
    };

    std::deque<ElementType> getRemains() {
        return dataProc;
    }

    int getStateCount() const {
        return static_cast<int>(offsets.size()) - 1;
    }

    std::vector<std::tuple<int, Condition, ActionPtr>> getTransitionsFrom(int state) const {
        std::vector<std::tuple<int, Condition, ActionPtr>> result;
        if (state >= 0 && state < static_cast<int>(offsets.size()) - 1) {
            int start = offsets[state];
            int end = offsets[state + 1];
            for (int i = start; i < end; i++) {
                result.emplace_back(matrixStates[i], matrixConditions[i], matrixActions[i]);
            }
        }
        return result;
    }

    bool isFinalState(int state) const {
        return std::find(finalStates.begin(), finalStates.end(), state) != finalStates.end();
    }

    std::vector<int> getFinalStates() const {
        return finalStates;
    }

    Harvester getHarvester() const {
        return outputsHarvester;
    }

	void genElemsOfConditions(Data* alphabet) {
		for (int i = 0; i < matrixConditions.size(); i++) {
            elemsConditions.push_back(std::vector<ElementType>());
			for (ElementType elem : *alphabet) {
                if (matrixConditions[i](elem)) {
                    elemsConditions[i].push_back(elem);
                }
                
			}
		}
	}

	Data genElem(std::mt19937& generator, Data* alphabet, float prop, int startState = 0) {
        if (elemsConditions.empty()) {
            return Data();
        }
        currentState = startState;

        Data res;
		
        while (true) {
			int condStart = offsets[currentState];
			int condEnd = offsets[currentState + 1];
            if (condStart == condEnd) {
                break;
            }
            if (std::count(finalStates.begin(), finalStates.end(), currentState) > 0) {
                std::uniform_int_distribution<> dist0(0, 1);
                if (dist0(generator) == 1) {
                    break;
                }
            }

            std::uniform_int_distribution<> dist1(condStart, condEnd - 1);
            int i = dist1(generator);
            std::uniform_int_distribution<> dist2(0, elemsConditions[i].size() - 1);
            int j = dist2(generator);

            res.push_back(elemsConditions[i][j]);
            currentState = matrixStates[i];
        }

		std::uniform_real_distribution<double> dist(0.0, 1.0);
		if (dist(generator) <= prop) {
            std::uniform_int_distribution<> dist1(0, res.size() - 1);
            std::uniform_int_distribution<> dist2(0, alphabet->size());
            res[dist1(generator)] = (*alphabet)[dist2(generator)];
		}
		
        return res;       
		
	};



    template<typename D, typename AO>
    friend FSM<D, AO> mergeFSM(
        const FSM<D, AO>& fsm1, int state1,
        const FSM<D, AO>& fsm2, int state2,
        std::function<void(AO)> harvester
    );
};


template<typename Data, typename ActionOut>
FSM<Data, ActionOut> mergeFSM(
    const FSM<Data, ActionOut>& fsm1, int state1,
    const FSM<Data, ActionOut>& fsm2, int state2,
    std::function<void(ActionOut)> harvester = nullptr
) {
    using Condition = typename FSM<Data, ActionOut>::Condition;
    using ActionPtr = typename FSM<Data, ActionOut>::ActionPtr;

    int n1 = fsm1.getStateCount();
    int n2 = fsm2.getStateCount();

    int totalStates = n1 + n2 - 1;

    std::unordered_map<int, std::vector<std::tuple<int, Condition, ActionPtr>>> transitions;


    for (int s = 0; s < n1; s++) {
        auto trans = fsm1.getTransitionsFrom(s);
        for (const auto& [to, cond, act] : trans) {
            transitions[s].emplace_back(to, cond, act);
        }
    }

	auto remapState = [&](int s) -> int {
		if (s == state2) {
			return state1;
		}
		else if (s < state2) {
			return s + n1;
		}
		else {
			return s + n1 - 1;
		}
		};

    for (int s = 0; s < n2; s++) {
        auto trans = fsm2.getTransitionsFrom(s);
        int fromRemapped = remapState(s);

        for (const auto& [to, cond, act] : trans) {
            int toRemapped = remapState(to);
            transitions[fromRemapped].emplace_back(toRemapped, cond, act);
        }
    }

    std::vector<int> finalStates;

    for (int fs : fsm1.getFinalStates()) {
        finalStates.push_back(fs);
    }

    for (int fs : fsm2.getFinalStates()) {
        int remapped = remapState(fs);
        if (std::find(finalStates.begin(), finalStates.end(), remapped) == finalStates.end()) {
            finalStates.push_back(remapped);
        }
    }
    return FSM<Data, ActionOut>(totalStates, std::move(transitions), std::move(finalStates), harvester);
}


FSM<std::wstring, wchar_t> fsmFromWstr(std::wstring data, Action<wchar_t(wchar_t)>* allAction = nullptr, std::function<void(wchar_t)> harvester = nullptr) {
    int n = data.size();
    std::vector<std::vector<std::function<bool(wchar_t)>>> conditions(n + 1, std::vector<std::function<bool(wchar_t)>>(n + 1, nullptr));
    std::vector<std::vector<Action<wchar_t(wchar_t)>*>> actions(n + 1, std::vector<Action<wchar_t(wchar_t)>*>(n + 1, allAction));
    std::vector<int> finals({n});

    for (int i = 0; i < n; i++) {
        conditions[i][i+1] = [value = data[i]](wchar_t c) {return c == value; };
    }

    FSM<std::wstring, wchar_t> result(conditions, actions, finals, harvester);

    return result;
}
