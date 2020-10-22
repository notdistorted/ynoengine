/*
 * This file is part of EasyRPG Player.
 *
 * EasyRPG Player is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * EasyRPG Player is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EasyRPG Player. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EP_SCENE_BATTLE_RPG2K3_H
#define EP_SCENE_BATTLE_RPG2K3_H

// Headers
#include "scene_battle.h"
#include "async_handler.h"
#include "window_actorsp.h"

/**
 * Scene_Battle class.
 * Manages the battles.
 */
class Scene_Battle_Rpg2k3 : public Scene_Battle {
public:
	enum BattleActionState {
		/**
		 * 1st action, called repeatedly.
		 * Handles healing of conditions that get auto removed after X turns.
		 */
		BattleActionState_ConditionHeal,
		/**
		 * 2nd action, called once.
		 * Used to execute the algorithm and print the first start line.
		 */
		BattleActionState_Execute,
		/**
		 * 3rd action, called once.
		 * Used to apply the new conditions, play an optional battle animation and sound, and print the second line of a technique.
		 */
		BattleActionState_Apply,
		/**
		 * Used to process reflected battle animation.
		 */
		BattleActionState_Reflect,
		/**
		* 4th action, called repeatedly.
		* Used for the results, concretely wait a few frames and pop the messages.
		*/
		BattleActionState_ResultPop,
		/**
		 * 5th action, called repeatedly.
		 * Used to push the message results, effects and advances the messages. If it finishes, it calls Death. If not, it calls ResultPop
		 */
		BattleActionState_ResultPush,
		/**
		 * 6th action, called once.
		 * Action treating whether the enemy died or not.
		 */
		BattleActionState_Death,
		/**
		 * 7th action, called once.
		 * It finishes the action and checks whether to repeat it if there is another target to hit.
		 */
		BattleActionState_Finished
	};

public:
	explicit Scene_Battle_Rpg2k3(const BattleArgs& args);
	~Scene_Battle_Rpg2k3() override;

	void Start() override;
	void Update() override;

protected:
	void Start2();
	void InitAtbGauge(Game_Battler& battler, int preempt_atb, int ambush_atb);
	void InitBattleCondition(lcf::rpg::System::BattleCondition condition);
	void InitEnemies();
	void InitActors();
	void InitAtbGauges();

	void UpdateEnemiesDirection();
	void UpdateActorsDirection();

	void OnSystem2Ready(FileRequestResult* result);
	void SetupSystem2Graphics();
	void CreateUi() override;

	void CreateBattleTargetWindow();
	void CreateBattleCommandWindow();

	void UpdateCursors() override;
	void DrawFloatText(int x, int y, int color, StringView text);

	void RefreshCommandWindow();

	void SetState(Scene_Battle::State new_state) override;

	bool CheckWin();
	bool CheckLose();
	bool CheckResultConditions();

	void ProcessActions() override;
	bool ProcessBattleAction(Game_BattleAlgorithm::AlgorithmBase* action);
	void FaceTarget(Game_Actor& source, const Game_Battler& target);
	void ProcessInput() override;

	void OptionSelected();
	void CommandSelected();
	void AttackSelected() override;
	void SubskillSelected();
	void SpecialSelected();
	void RowSelected();

	void Escape(bool force_allow = false);

	void SelectNextActor();

	void ActionSelectedCallback(Game_Battler* for_battler) override;

	void ShowNotification(std::string text);

	bool IsEscapeAllowed() const = delete; // disable accidental calls to base class version
	bool IsEscapeAllowedFromOptionWindow() const;
	bool IsEscapeAllowedFromActorCommand() const;

	bool CheckAnimFlip(Game_Battler* battler);

	void SetWait(int min_wait, int max_wait);
	bool CheckWait();

	std::unique_ptr<Sprite> ally_cursor, enemy_cursor;

	struct FloatText {
		std::shared_ptr<Sprite> sprite;
		int remaining_time = 30;
	};

	std::vector<FloatText> floating_texts;
	int battle_action_wait = 0;
	int battle_action_min_wait = 0;
	int battle_action_state = BattleActionState_Execute;
	bool battle_action_need_event_refresh = true;
	int combo_repeat = 1;

	std::unique_ptr<Window_BattleStatus> enemy_status_window;
	std::unique_ptr<Window_ActorSp> sp_window;

	std::vector<Game_Battler*> targets;

	int select_target_flash_count = 0;

	FileRequestBinding request_id;
	std::shared_ptr<Game_BattleAlgorithm::AlgorithmBase> pending_battle_action = {};
	bool first_strike = false;

	int start_message_shown = 0;
	bool escape_initiated = false;
	bool win_wait_elapsed = false;
	bool lose_wait_elapsed = false;
	bool auto_battle = false;
};

#endif
