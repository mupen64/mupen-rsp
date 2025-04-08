/*
 * Copyright (c) 2025, hacktarux-azimer-rsp-hle maintainers, contributors, and original authors (Hacktarux, Azimer).
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "stdafx.h"
#include "Config.h"
#include "helpers/io_helpers.h"

#define CONFIG_PATH "hacktarux-azimer-rsp-hle.cfg"

t_config config = {};
core_plugin_cfg g_cfg{};
t_config default_config = {};
const wchar_t* cfg_groups[] = {L"General"};
int32_t g_config_readonly;

core_plugin_cfg_item cfg_items[] = {
{0,
 pcit_bool,
 L"Audio HLE",
 L"Whether audio lists are processed externally",
 0,
 nullptr,
 &g_config_readonly,
 &config.audio_hle,
 &default_config.audio_hle},
{0,
 pcit_bool,
 L"Graphics HLE",
 L"Whether display lists are processed externally",
 0,
 nullptr,
 &g_config_readonly,
 &config.graphics_hle,
 &default_config.graphics_hle},
{
0,
pcit_bool,
L"Audio External",
L"Whether audio lists are processed by the audio plugin specified by 'Audio Path'",
0,
nullptr,
&g_config_readonly,
&config.audio_external,
&default_config.audio_external,
},
{
0,
pcit_path,
L"Audio Path",
L"Path to the external audio plugin path for alist processing",
0,
nullptr,
&g_config_readonly,
config.audio_path,
default_config.audio_path,
},
{
0,
pcit_bool,
L"Ucode Cache Verify",
L"Verify the cached ucode function on every audio ucode task. Enable this if you are debugging dynamic ucode changes.",
0,
nullptr,
&g_config_readonly,
&config.ucode_cache_verify,
&default_config.ucode_cache_verify,
},
};

void config_init()
{
    g_cfg = {
    .groups_len = std::size(cfg_groups),
    .groups = cfg_groups,
    .items_len = std::size(cfg_items),
    .items = cfg_items,
    };
}

void config_save()
{
    printf("[RSP] Saving config...\n");
    FILE* f = fopen(CONFIG_PATH, "wb");
    if (!f)
    {
        printf("[RSP] Can't save config\n");
        return;
    }
    fwrite(&config, sizeof(t_config), 1, f);
    fclose(f);
}

void config_load()
{
    printf("[RSP] Loading config...\n");

    auto buffer = read_file_buffer(CONFIG_PATH);
    t_config loaded_config;

    if (buffer.empty() || buffer.size() != sizeof(t_config))
    {
        // Failed, reset to default
        printf("[RSP] No config found, using default\n");
        memcpy(&loaded_config, &default_config, sizeof(t_config));
    }
    else
    {
        uint8_t* ptr = buffer.data();
        memread(&ptr, &loaded_config, sizeof(t_config));
    }

    if (loaded_config.version < default_config.version)
    {
        // Outdated version, reset to default
        printf("[RSP] Outdated config version, using default\n");
        memcpy(&loaded_config, &default_config, sizeof(t_config));
    }

    memcpy(&config, &loaded_config, sizeof(t_config));
}
