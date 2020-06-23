# MiniViews
An application providing small, transparent views of windows in the backround

Demonstration Video:

[![Demonstration Video](http://img.youtube.com/vi/bkwBFb_JDBQ/0.jpg)](http://www.youtube.com/watch?v=bkwBFb_JDBQ)

## General Operations
- **Add a Mini View** to the screen by using the hotkey (Ctrl Shift A), the button in the General Tab, or from the notification icon's menu. Once added a Mini View can be dragged and dropped (either by the title bar or the client area) onto the window you wish to keep a persistent view of.

- **Toggle Edit Mode** using the hotkey (Ctrl Shift E), the checkbox in the General Tab, or from the notification icon's menu. A Mini View may be dragged to any position and re-sized while in edit mode; when not in edit mode the Mini Views' borders will be cut and the Mini View will become possible to click through.

- **Close a Mini View** using the x button on their title bar or by right clicking the Mini View and selecting close. These options are only available while in edit mode.

- **Close all open Mini Views** using the Clear Mini Views option found in the general tab and in the notification icon's menu.

- **Edit Opacity** using the hotkeys (Ctrl Shift + to increase, Ctrl Shift - to decrease). Increasing the opacity will make the Mini View's contents more visible than items underneath, while decreasing will make the Mini View's contents less visible than the items underneath. At 255 opacity the Mini Views completely cover up the items below, while at 0 opacity the Mini Views are invisible.

- **Exit Mini Views** using the close button on the dialog or by right clicking the notification icon and selecting Exit

## Customizing Views
- **Resize a Mini View** by click-and-dragging its border (while in Edit Mode) or by setting the width and height of a particular Mini View in the Views tab.

- **Move a Mini View** by click-and-dragging its center (while in Edit Mode) or by setting the X-Pos and Y-Pos in the Views tab.

- **Match the Source Windows Size/Position** by right clicking a Mini View and selecting Match Source or by pressing the Match Source Size or Match Source Position buttons in the Views tab.

- **Set a Mini Views Clipping Region** - the area of the source window you wish to view - by right clicking a Mini View and selecting Set Clip Region or by pressing the 'Set' button in the Clip Settings portion of the Views tab. You can also adjust this area by setting the Left, Top, Right, and Bottom coordinates of the clip area in the Views tab.

- **Clear a Mini Views Clipping Region** by right clicking a Mini View and selecting Clear Clip Region or by un-checking the 'Clipped' check box in the Views tab.

- **Lock or Unlock a Mini Views Size Ratio** by right clicking a Mini View and selecting Lock Size Ratio or by clicking the 'Lock Size Ratio' check box in the Views tab. This setting keeps the Mini Views width and height in the same proportion as the width and height of the source window (or clipped region of the source window, if clipped).

- **Toggle Hiding a Mini View when its source is focused** by right clicking a Mini View and selecting Hide When Source is Top or by clicking the 'Hide When Source Is Top' check box in the Views tab.

## Advanced Options
- **Toggle Showing the Mini Views Dialog on Start** using the check box in the Advanced tab.
		
- **Toggle Use Notification Icon** using the check box in the Advanced tab. You may need to restart the program for this to take effect.

- **Toggle Minimize to Notification Icon** using the check box in the Advanced tab. This will not occur if Use Notification Icon is not set.

- **Toggle Confirm Exit by Dialog** using the check box in the Advanced tab. When set this option requires the user to confirm exiting Mini Views when closing the Main Dialog. This setting helps avoid confusion with other applications that minimize to Notification Icons when their window is closed.

## Uninstall
If you wish to delete all saved preferences, open the dialog and hit the 'Clear Saved Settings' button in the Advanced tab. Alternatively, advanced users may navigate to HKEY_CURRENT_USER\Software\ in the registry and delete the 'Mini Views' key. To finish your uninstall simply delete the MiniViews.exe or MiniViews (free).exe file.
