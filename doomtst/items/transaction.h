#include "../game/ecs/game_object.h"
#pragma once
namespace items {
	struct Plan {
		virtual void apply(ecs::Ecs& world) = 0;
		virtual ~Plan() = default;
	};

	template<typename T>
	concept PlanType= std::derived_from<T, Plan>;
	struct Operation {
		virtual stn::Option<stn::box<Plan>> plan(ecs::Ecs& world) = 0;
		virtual ~Operation() = default;
	};
	template<typename T>
	concept OperationType = requires(T& oper,ecs::Ecs& world) {

		{
			oper.plan(world)
		}->stn::OptionType;
	}
	&&PlanType<typename decltype(std::declval<T&>().plan(std::declval<ecs::Ecs&>()))::value_type>;

	template<OperationType T>
	using PlanForOperation = typename decltype(std::declval<T&>().plan(std::declval<ecs::Ecs&>()))::value_type;
	using PotentialPlan = stn::Option<stn::box<Plan>>;
	template<OperationType T>
	struct OperationExecutor:Operation {
		using plan_type = PlanForOperation<T>;
		T operation;
		PotentialPlan plan(ecs::Ecs& world) {
			stn::Option<plan_type> plan=operation.plan(world);
			if (!plan) {
				return stn::None;
			}
			else {
				return stn::box<Plan>(stn::construct_derived<plan_type>(), std::move(plan.unwrap()));
			}
		}
		template<typename ...Args> requires std::constructible_from<T,Args&&...>
		OperationExecutor(Args&&... args) :operation(std::forward<Args>(args)...) {
		}
	};


	using TransactionBox = stn::box<Operation>;

	template<OperationType T>
	inline void stage_transaction(ecs::Ecs& world,T&& transaction) {
		world.write_command(std::move(TransactionBox(stn::construct_derived<OperationExecutor<T>>(),std::move(transaction))));
	}
	template<OperationType T,typename ...Args> requires std::constructible_from<OperationExecutor<T>,Args...>
	inline void stage_transaction_emplaced(ecs::Ecs& world,Args&&... args) {
		world.write_command(TransactionBox(stn::construct_derived<OperationExecutor<T>>(), std::forward<Args>(args)...));
	}
	struct TransactionApplier :ecs::System {
		void run(ecs::Ecs& world) {
			for (TransactionBox& element: world.read_commands< TransactionBox>()) {
				PotentialPlan plan = element->plan(world);
				if (plan) {
					plan.unwrap()->apply(world);
				}
			}
		}
	};
	struct TransactionPlugin :Core::Plugin {
		void build(Core::App& app) {
			app.emplace_system<TransactionApplier>();
		}

	};
}