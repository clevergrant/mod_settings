#include "IRuntimeVariable.hpp"
#include "ScriptDefinitions/ScriptProperty.hpp"

namespace ModSettings {

IRuntimeVariable::IRuntimeVariable() {
  bitfield.isInPreGame = true;
  bitfield.isInGame = true;
  bitfield.isVisible = true;
  bitfield.isInitialized = true;
  bitfield.isDisabled = false;
  bitfield.canBeRestoredToDefault = true;
}

IRuntimeVariable::IRuntimeVariable(ScriptProperty* prop) : IRuntimeVariable() {
  this->updatePolicy = user::EConfigVarUpdatePolicy::ConfirmationRequired;
  this->name = prop->name;

  // prop->ReadProperty("ModSettings.updatePolicy", &updatePolicy, user::EConfigVarUpdatePolicy::ConfirmationRequired);
  prop->ReadProperty("ModSettings.mod", &this->modName);
  prop->ReadProperty("ModSettings.displayName", &this->displayName, this->name);
  prop->ReadProperty("ModSettings.description", &this->description);
  prop->ReadProperty<uint32_t>("ModSettings.order", &this->order, (uint32_t)-1);
  this->className = prop->parent->name;

  // gwheel patch: ModSettings.hidden ("true" / "1") suppresses UI rendering of
  // this field while leaving it discoverable as a `ModSettings.dependency`
  // target. Direct string compare bypasses the type-aware FromString template
  // path (which mis-parses non-CName/CString values for non-bool fields).
  auto hiddenStr = prop->runtimeProperties.Get("ModSettings.hidden");
  if (hiddenStr) {
    std::string s(hiddenStr->c_str());
    if (s == "true" || s == "1") {
      this->bitfield.isVisible = false;
    }
  }
  
  char str[0x200];
  // std::sprintf(str, "/mods/%s/%s", prop->parent->name.ToString(), this->name.ToString());
  std::sprintf(str, "/mods/%s/%s", this->modName.ToString(), prop->parent->name.ToString());
  this->groupPath = CNamePool::Add(str);

}

IRuntimeVariable::IRuntimeVariable(CName modName, CName className, CName propertyName, CName displayName, CName description, uint32_t order) : IRuntimeVariable() {
  this->modName = modName;
  this->className = className;
  this->updatePolicy = user::EConfigVarUpdatePolicy::ConfirmationRequired;
  this->name = propertyName;
  this->displayName = displayName;
  char str[0x200];
  std::sprintf(str, "/mods/%s/%s", modName.ToString(), className.ToString());
  // std::sprintf(str, "/mods/%s/%s", className.ToString(), propertyName.ToString());
  this->groupPath = CNamePool::Add(str);
  this->description = description;
  this->order = order;
}

}