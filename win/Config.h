#pragma once

struct t_config
{
    size_t version = 0;
    /**
     * \brief Whether audio lists are processed externally
     */
    bool AudioHle = false;
    /**
     * \brief Whether display lists are processed externally
     */
    bool GraphicsHle = true;
    /**
      * \brief Whether audio lists are processed by the audio plugin specified in audioname
      */
    bool SpecificHle = false;
    /**
      * \brief Path to the plugin 
      */
    char audioname[100] = {0};
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
