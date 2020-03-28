/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#include "Shibboleth_StateMachineResource.h"
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>

SHIB_REFLECTION_DEFINE_BEGIN(StateMachineResource)
	.classAttrs(
		CreatableAttribute(),
		ResExtAttribute(".state_machine.bin"),
		ResExtAttribute(".state_machine"),
		MakeLoadFileCallbackAttribute(&StateMachineResource::loadStateMachine)
	)

	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(StateMachineResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(StateMachineResource)

static constexpr const char* const g_variable_names[static_cast<size_t>(Esprit::VariableSet::VariableType::Count)] = {
	"references",
	"strings",
	"floats",
	"integers",
	"bools"
};

const Esprit::StateMachine* StateMachineResource::getStateMachine(void) const
{
	return _state_machine.get();
}

void StateMachineResource::loadStateMachine(IFile* file)
{
	SerializeReaderWrapper readerWrapper;

	if (!OpenJSONOrMPackFile(readerWrapper, getFilePath().getBuffer(), file)) {
		LogErrorResource("Failed to load state machine '%s' with error: '%s'", getFilePath().getBuffer(), readerWrapper.getErrorText());
		failed();
		return;
	}

	ProxyAllocator allocator("Resource");
	_state_machine.reset(SHIB_ALLOCT(Esprit::StateMachine, allocator));

	const ReflectionManager& refl_mgr = GetApp().getReflectionManager();
	const Gaff::ISerializeReader& reader = *readerWrapper.getReader();

	// Load variables.
	{
		const auto guard_vars = reader.enterElementGuard("variables");

		if (!reader.isNull()) {
			if (reader.isObject()) {
				for (int32_t i = 0; i < static_cast<int32_t>(Esprit::VariableSet::VariableType::Count); ++i) {
					const auto guard_var_type = reader.enterElementGuard(g_variable_names[i]);

					if (reader.isNull()) {
						continue;
					}

					if (!reader.isArray()) {
						// $TODO: Log error.
						continue;
					}

					reader.forEachInArray([&](int32_t) -> bool
					{
						if (!reader.isString()) {
							// $TODO: Log error.
							return false;
						}

						const char* const name = reader.readString();

						if (!_state_machine->getVariables().addVariable(Esprit::HashString32<>(name), static_cast<Esprit::VariableSet::VariableType>(i))) {
							// $TODO: Log error (duplicate name).
						}

						reader.freeString(name);
						return false;
					});
				}
	
			} else {
				// $TODO: Log error.
			}
		}
	}

	// Load states and processes.
	{
		const auto guard_states = reader.enterElementGuard("states");

		if (reader.isNull()) {
			// $TODO: Log error.

		} else {
			const auto* const process_bucket = refl_mgr.getTypeBucket(CLASS_HASH(Esprit::IProcess));

			if (!process_bucket) {
				// $TODO: Log error.
				_state_machine.reset(nullptr);
				failed();
				return;
			}

			if (reader.isObject() && reader.size() > 0) {
				reader.forEachInObject([&](const char* key) -> bool
				{
					const Esprit::HashString32<> name(key);

					if (!_state_machine->addState(name)) {
						// $TODO: Log error. (duplicate name)
						return false;
					}

					const int32_t index = _state_machine->getStateIndex(name);

					{
						const auto guard_process = reader.enterElementGuard("processes");

						if (!reader.isNull()) {
							if (reader.isArray()) {
								reader.forEachInArray([&](int32_t) -> bool
								{
									const Gaff::IReflectionDefinition* ref_def = nullptr;

									{
										const auto guard_type = reader.enterElementGuard("process_type");

										if (reader.isString()) {
											const char* const type = reader.readString();
											const Gaff::Hash64 type_hash = Gaff::FNV1aHash64String(type);
											reader.freeString(type);

											const auto it = Gaff::LowerBound(*process_bucket, type_hash, ReflectionManager::CompareRefHash);

											if (it == process_bucket->end() || (*it)->getReflectionInstance().getHash() == type_hash) {
												ref_def = *it;
											}

										} else {
											// $TODO: Log error.
										}
									}

									if (!ref_def) {
										// $TODO: Log error.
										return false;
									}

									Esprit::IProcess* const process = ref_def->createT<Esprit::IProcess>(CLASS_HASH(Esprit::IProcess), allocator);

									if (process) {
										if (ref_def->load(reader, ref_def->getBasePointer(CLASS_HASH(Esprit::IProcess), process))) {
											_state_machine->addProcess(index, process);
										} else {
											// $TODO: Log error.
										}

									} else {
										// $TODO: Log error.
									}

									return false;
								});

							} else {
								// $TODO: Log error.
							}
						}
					}

					return false;
				});

			} else {
				// $TODO: Log error.
			}
		}
	}

	// Load edges and conditions.
	{
		const auto guard_edges = reader.enterElementGuard("edges");

		if (reader.isNull()) {
			// $TODO: Log error.

		} else {
			const auto* const cond_bucket = refl_mgr.getTypeBucket(CLASS_HASH(Esprit::ICondition));

			if (!cond_bucket) {
				// $TODO: Log error.
				_state_machine.reset(nullptr);
				failed();
				return;
			}

			if (reader.isArray() && reader.size() > 0) {
				reader.forEachInArray([&](int32_t) -> bool
				{
					int32_t source_index = -1;
					int32_t dest_index = -1;

					{
						const auto guard_src = reader.enterElementGuard("source");

						if (reader.isString()) {
							const char* const source = reader.readString();
							source_index = _state_machine->getStateIndex(Esprit::HashStringTemp32<>(source));
							reader.freeString(source);
						} else {
							// $TODO: Log error.
						}
					}

					{
						const auto guard_dest = reader.enterElementGuard("destination");

						if (reader.isString()) {
							const char* const destination = reader.readString();
							dest_index = _state_machine->getStateIndex(Esprit::HashStringTemp32<>(destination));
							reader.freeString(destination);
						} else {
							// $TODO: Log error.
						}
					}

					if (source_index < 0) {
						// $TODO: Log error.
					}

					if (dest_index < 0) {
						// $TODO: Log error.
					}

					if (source_index > -1 && dest_index > -1) {
						const int32_t edge_index = _state_machine->addEdge(source_index, dest_index);

						if (edge_index > -1) {
							const auto guard_conditions = reader.enterElementGuard("conditions");

							if (reader.isArray()) {
								reader.forEachInArray([&](int32_t) -> bool
								{
									const Gaff::IReflectionDefinition* ref_def = nullptr;

									{
										const auto guard_type = reader.enterElementGuard("condition_type");

										if (reader.isString()) {
											const char* const type = reader.readString();
											const Gaff::Hash64 type_hash = Gaff::FNV1aHash64String(type);
											reader.freeString(type);

											const auto it = Gaff::LowerBound(*cond_bucket, type_hash, ReflectionManager::CompareRefHash);

											if (it == cond_bucket->end() || (*it)->getReflectionInstance().getHash() == type_hash) {
												ref_def = *it;
											}

										} else {
											// $TODO: Log error.
										}
									}

									if (!ref_def) {
										// $TODO: Log error.
										return false;
									}

									Esprit::ICondition* const condition = ref_def->createT<Esprit::ICondition>(CLASS_HASH(Esprit::ICondition), allocator);

									if (condition) {
										if (ref_def->load(reader, ref_def->getBasePointer(CLASS_HASH(Esprit::ICondition), condition))) {
											_state_machine->addCondition(source_index, edge_index, condition);
										} else {
											// $TODO: Log error.
										}
									} else {
										// $TODO: Log error.
									}

									return false;
								});
							}
						}
					}

					return false;
				});

			} else {
				// $TODO: Log error.
			}
		}
	}

	if (_state_machine->finalize()) {
		succeeded();
	} else {
		_state_machine.reset(nullptr);
		// $TODO: Log error.
		failed();
	}
}

NS_END
