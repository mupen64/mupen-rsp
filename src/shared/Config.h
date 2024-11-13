#pragma once

#define PLUGIN_NAME "Mupen64 HLE RSP Plugin 0.3.0"

struct t_config
{
    size_t version = 1;
    /**
     * \brief Whether audio lists are processed externally
     */
    bool audio_hle = false;
    /**
     * \brief Whether display lists are processed externally
     */
    bool graphics_hle = true;
    /**
      * \brief Whether audio lists are processed by the audio plugin specified in audioname
      */
    bool audio_external = false;
    /**
      * \brief Verify the cached ucode function on every audio ucode task. Enable this if you are debugging dynamic ucode changes.  
      */
    bool ucode_cache_verify = false;
    /**
      * \brief Path to the external audio plugin path for alist processing 
      */
    char audio_path[260] = {0};
};

extern t_config config;

/**
 * \brief Saves the config
 */
void config_save();

/**
 * \brief Loads the config
 */
void config_load();
