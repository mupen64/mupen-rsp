#pragma once

/*
 *	Interface for calls originating from core or shared layer to the view layer
 *
 *	Must be implemented in the view layer.
 */
namespace FrontendService
{
    /**
     * \brief Shows the user an error dialog
     * \param str The dialog content
     * \param title The dialog title
     * \param hwnd The parent window
     */
    void show_error(const char* str, const char* title = nullptr, void* hwnd = nullptr);

    /**
     * \brief Shows the user an information dialog
     * \param str The dialog content
     * \param title The dialog title
     * \param hwnd The parent window
     */
    void show_info(const char* str, const char* title = nullptr, void* hwnd = nullptr);
}
