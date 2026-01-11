#include "../../game/ecs/game_object.h"
#pragma once
namespace items {

	struct Transaction {
		ecs::entity first;
		ecs::entity second;
		virtual bool validate(const ecs::Ecs& world)const = 0;
		virtual void apply(ecs::Ecs& world)=0;

	};

	template<typename T>
	concept TransactionType = std::derived_from<T, Transaction>;
	using TransactionBox = stn::box<Transaction>;
	template<TransactionType T> 
	void stage_transaction(ecs::Ecs& world,T&& transaction) {

		world.write_command(TransactionBox(stn::construct_derived<T>(),std::move(transaction)));

	}
	template<TransactionType T,typename ...Args> requires std::constructible_from<T,Args&&...>
	void stage_transaction_emplaced(ecs::Ecs& world,Args&&... args) {
		world.write_command(TransactionBox(stn::construct_derived<T>(), std::forward<Args>(args)...));
	}
	struct TransactionApplier :ecs::System {
		void run(ecs::Ecs& world) {
			for (TransactionBox& element: world.read_commands< TransactionBox>()) {
				if (element->validate(world)) {
					element->apply(world);
				}
			}
		}
	};
}