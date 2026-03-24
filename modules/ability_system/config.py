def can_build(env, platform):
    return True


def configure(env):
    env.Append(CPPDEFINES=["ABILITY_SYSTEM_MODULE"])


def get_doc_classes():
    return [
        "AbilitySystem",
        "ASAbility",
        "ASAbilityPhase",
        "ASContainer",
        "ASAbilitySpec",
        "ASAttribute",
        "ASAttributeSet",
        "ASComponent",
        "ASCue",
        "ASCueAnimation",
        "ASCueAudio",
        "ASCueSpec",
        "ASDelivery",
        "ASEditorPlugin",
        "ASEditorPropertyName",
        "ASEditorPropertySelector",
        "ASEditorPropertyTagSelector",
        "ASEffect",
        "ASEffectSpec",
        "ASEventTag",
        "ASInspectorPlugin",
        "ASPackage",
        "ASTagSpec",
        "ASStateSnapshot",
        "ASTagsPanel",
    ]


def get_doc_path():
    return "doc_classes"


def get_icon_path():
    return "icons"
