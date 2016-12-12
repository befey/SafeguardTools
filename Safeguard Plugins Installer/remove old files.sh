#!/bin/sh

#Clean up some old files
rm -Rf /Applications/Adobe\ Illustrator\ CC\ 2015/Plug-ins.localized/ColorTools.aip
rm -Rf /Applications/Adobe\ Illustrator\ CC\ 2015/Plug-ins.localized/TextTools.aip

#3/24/16 -- Renamed to SafeguardTools and consolidated all plugins to one file
rm -Rf /Applications/Adobe\ Illustrator\ CC\ 2015/Plug-ins.localized/Safeguard/ColorTools.aip
rm -Rf /Applications/Adobe\ Illustrator\ CC\ 2015/Plug-ins.localized/Safeguard/TextTools.aip

#4/15/16 -- Renamed extension folder to SafeguardTools
rm -Rf /Library/Application\ Support/Adobe/CEP/extensions/com.gosafeguard.ColorTools

#6/27/16 -- Delete files in pre-CC2015.3 folders.
rm -Rf /Library/Application\ Support/Adobe/CEP/extensions/com.gosafeguard.SafeguardTools
rm -Rf /Applications/Adobe\ Illustrator\ CC\ 2015/Plug-ins.localized/Safeguard

#12/12/16 -- Delete files in pre-CC2017 folders.
rm -Rf /Applications/Adobe\ Illustrator\ CC\ 2015.3/CEP/extensions/com.gosafeguard.SafeguardTools
rm -Rf /Applications/Adobe\ Illustrator\ CC\ 2015.3/Plug-ins.localized/Safeguard
rm -Rf /Applications/Adobe\ Illustrator\ CC\ 2017/Plug-ins.localized/ColorTools.aip
rm -Rf /Applications/Adobe\ Illustrator\ CC\ 2017/Plug-ins.localized/TextTools.aip
