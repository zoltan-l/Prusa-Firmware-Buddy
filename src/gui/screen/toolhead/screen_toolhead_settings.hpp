#pragma once

#include <variant>
#include <cstdint>

#include "screen_toolhead_settings_common.hpp"

#include <window_menu_enum_switch.hpp>
#include <config_store/store_definition.hpp>

namespace screen_toolhead_settings {

class MI_NOZZLE_DIAMETER final : public MI_TOOLHEAD_SPECIFIC_SPIN<MI_NOZZLE_DIAMETER> {
public:
    MI_NOZZLE_DIAMETER(Toolhead toolhead = default_toolhead);
    static float read_value_impl(ToolheadIndex ix);
    static void store_value_impl(ToolheadIndex ix, float set);
};

class MI_NOZZLE_DIAMETER_HELP : public IWindowMenuItem {
public:
    MI_NOZZLE_DIAMETER_HELP();
    void click(IWindowMenu &menu) override;
};

#if HAS_HOTEND_TYPE_SUPPORT()
// TODO: This should be migrated into MI_TOOLHEAD_SPECIFIC, but the config store stores it as a single flag, so no need for now
class MI_HOTEND_TYPE : public WiStoreEnumSwitch<&config_store_ns::CurrentStore::hotend_type> {
public:
    MI_HOTEND_TYPE();
};

// TODO: This should be migrated into MI_TOOLHEAD_SPECIFIC, but the config store stores it as a single flag, so no need for now
class MI_NOZZLE_SOCK : public WI_ICON_SWITCH_OFF_ON_t {
public:
    MI_NOZZLE_SOCK();
    void OnChange(size_t old_index) override;
};

using MI_HOTEND_SOCK_OR_TYPE = std::conditional_t<hotend_type_only_sock, MI_NOZZLE_SOCK, MI_HOTEND_TYPE>;
#endif

using ScreenToolheadDetail_ = ScreenMenu<EFooter::Off,
    MI_RETURN,
    MI_NOZZLE_DIAMETER
#if PRINTER_IS_PRUSA_XL()
    // Prusa XL was sold with .6mm nozzles and then with .4mm nozzles, so the users need to set in the FW what nozzles they have
    // This is to help them out a bit
    ,
    MI_NOZZLE_DIAMETER_HELP
#endif
#if HAS_HOTEND_TYPE_SUPPORT()
    ,
    MI_HOTEND_SOCK_OR_TYPE
#endif
    >;

class ScreenToolheadDetail : public ScreenToolheadDetail_ {
public:
    ScreenToolheadDetail(Toolhead toolhead = default_toolhead);

private:
    const Toolhead toolhead;
    StringViewUtf8Parameters<2> title_params;
};

class MI_TOOLHEAD : public IWindowMenuItem {
public:
    MI_TOOLHEAD(Toolhead toolhead);

protected:
    void click(IWindowMenu &) final;

private:
    const Toolhead toolhead;
    StringViewUtf8Parameters<2> label_params;
};

#if HAS_TOOLCHANGER()
template <typename>
struct ScreenToolheadSettingsList_;

template <size_t... i>
struct ScreenToolheadSettingsList_<std::index_sequence<i...>> {
    using T = ScreenMenu<GuiDefaults::MenuFooter, MI_RETURN, WithConstructorArgs<MI_TOOLHEAD, ToolheadIndex(i)>..., WithConstructorArgs<MI_TOOLHEAD, AllToolheads {}>>;
};

class ScreenToolheadSettingsList : public ScreenToolheadSettingsList_<std::make_index_sequence<toolhead_count>>::T {
public:
    ScreenToolheadSettingsList();
};
#endif

} // namespace screen_toolhead_settings

using ScreenToolheadDetail = screen_toolhead_settings::ScreenToolheadDetail;

#if HAS_TOOLCHANGER()
using ScreenToolheadSettingsList = screen_toolhead_settings::ScreenToolheadSettingsList;
#endif
