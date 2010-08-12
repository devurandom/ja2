cmake_minimum_required(VERSION 2.4.6)
cmake_policy(SET CMP0005 OLD)

project( "ja2-stracciatella")

option(DEBUG "Debug build?" OFF)
option(JA2EDITOR "Build Editor?" OFF)

#build type
IF(DEBUG)
  set( CMAKE_BUILD_TYPE Debug)
  add_definitions( -D_DEBUG)
  add_definitions( -DSGP_DEBUG)
ELSE(DEBUG)
  set( CMAKE_BUILD_TYPE Release)
ENDIF(DEBUG)

#language defines uncomment exactly one
add_definitions(
#  -DGERMAN
#  -DDUTCH
   -DENGLISH
#  -DFRENCH
#  -DITALIAN
#  -DPOLISH
# Russian BUKA Agonia Vlasti release
#  -DRUSSIAN
# Russian Gold release
#  -DRUSSIAN_GOLD
)

#code defines
add_definitions( -DJA2 -DSGPDATADIR=\"/usr/local/share/ja2\")

#c-compiler flags
set( CMAKE_C_FLAGS ${CMAKE_C_FLAGS}
  "-std=gnu99"
)

#cxx-compiler flags
#set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}
#)

find_Package(SDL REQUIRED)
IF(MINGW)
  set( SDL_INCLUDE_DIR "C:\\msys\\1.0\\local\\include\\SDL")
  set( SDL_LIBRARY ${SDL_LIBRARY} SDLmain)
ENDIF(MINGW)

include_directories(${CMAKE_SOURCE_DIR}/Build)
include_directories(${CMAKE_SOURCE_DIR}/Build/Tactical)
include_directories(${CMAKE_SOURCE_DIR}/Build/Strategic)
include_directories(${CMAKE_SOURCE_DIR}/Build/Editor)
include_directories(${CMAKE_SOURCE_DIR}/Build/Res)
include_directories(${CMAKE_SOURCE_DIR}/Build/Laptop)
include_directories(${CMAKE_SOURCE_DIR}/Build/Utils)
include_directories(${CMAKE_SOURCE_DIR}/Build/TileEngine)
include_directories(${CMAKE_SOURCE_DIR}/Build/TacticalAI)
include_directories(${CMAKE_SOURCE_DIR}/sgp)
include_directories(${SDL_INCLUDE_DIR})

set( JA2SRCS
  Build/AniViewScreen.cc
  Build/Credits.cc
  Build/Fade_Screen.cc
  Build/GameInitOptionsScreen.cc
  Build/GameLoop.cc
  Build/GameScreen.cc
  Build/GameSettings.cc
  Build/GameVersion.cc
  Build/HelpScreen.cc
  Build/Init.cc
  Build/Intro.cc
  Build/JA2_Splash.cc
  Build/JAScreens.cc
  Build/Laptop/AIM.cc
  Build/Laptop/AIMArchives.cc
  Build/Laptop/AIMFacialIndex.cc
  Build/Laptop/AIMHistory.cc
  Build/Laptop/AIMLinks.cc
  Build/Laptop/AIMMembers.cc
  Build/Laptop/AIMPolicies.cc
  Build/Laptop/AIMSort.cc
  Build/Laptop/BobbyR.cc
  Build/Laptop/BobbyRAmmo.cc
  Build/Laptop/BobbyRArmour.cc
  Build/Laptop/BobbyRGuns.cc
  Build/Laptop/BobbyRMailOrder.cc
  Build/Laptop/BobbyRMisc.cc
  Build/Laptop/BobbyRShipments.cc
  Build/Laptop/BobbyRUsed.cc
  Build/Laptop/BrokenLink.cc
  Build/Laptop/CharProfile.cc
  Build/Laptop/EMail.cc
  Build/Laptop/Files.cc
  Build/Laptop/Finances.cc
  Build/Laptop/Florist.cc
  Build/Laptop/Florist_Cards.cc
  Build/Laptop/Florist_Gallery.cc
  Build/Laptop/Florist_Order_Form.cc
  Build/Laptop/Funeral.cc
  Build/Laptop/History.cc
  Build/Laptop/IMPVideoObjects.cc
  Build/Laptop/IMP_AboutUs.cc
  Build/Laptop/IMP_Attribute_Entrance.cc
  Build/Laptop/IMP_Attribute_Finish.cc
  Build/Laptop/IMP_Attribute_Selection.cc
  Build/Laptop/IMP_Begin_Screen.cc
  Build/Laptop/IMP_Compile_Character.cc
  Build/Laptop/IMP_Confirm.cc
  Build/Laptop/IMP_Finish.cc
  Build/Laptop/IMP_HomePage.cc
  Build/Laptop/IMP_MainPage.cc
  Build/Laptop/IMP_Personality_Entrance.cc
  Build/Laptop/IMP_Personality_Finish.cc
  Build/Laptop/IMP_Personality_Quiz.cc
  Build/Laptop/IMP_Portraits.cc
  Build/Laptop/IMP_Text_System.cc
  Build/Laptop/IMP_Voices.cc
  Build/Laptop/Insurance.cc
  Build/Laptop/Insurance_Comments.cc
  Build/Laptop/Insurance_Contract.cc
  Build/Laptop/Insurance_Info.cc
  Build/Laptop/Laptop.cc
  Build/Laptop/Mercs.cc
  Build/Laptop/Mercs_Account.cc
  Build/Laptop/Mercs_Files.cc
  Build/Laptop/Mercs_No_Account.cc
  Build/Laptop/Personnel.cc
  Build/Laptop/Store_Inventory.cc
  Build/LoadSaveEMail.cc
  Build/LoadSaveTacticalStatusType.cc
  Build/Loading_Screen.cc
  Build/MainMenuScreen.cc
  Build/MessageBoxScreen.cc
  Build/Options_Screen.cc
  Build/SaveLoadGame.cc
  Build/SaveLoadScreen.cc
  Build/Screens.cc
  Build/Strategic/AI_Viewer.cc
  Build/Strategic/Assignments.cc
  Build/Strategic/Auto_Resolve.cc
  Build/Strategic/Campaign_Init.cc
  Build/Strategic/Creature_Spreading.cc
  Build/Strategic/Game_Clock.cc
  Build/Strategic/Game_Event_Hook.cc
  Build/Strategic/Game_Events.cc
  Build/Strategic/Game_Init.cc
  Build/Strategic/Hourly_Update.cc
  Build/Strategic/LoadSaveSectorInfo.cc
  Build/Strategic/LoadSaveStrategicMapElement.cc
  Build/Strategic/LoadSaveUndergroundSectorInfo.cc
  Build/Strategic/MapScreen.cc
  Build/Strategic/Map_Screen_Helicopter.cc
  Build/Strategic/Map_Screen_Interface.cc
  Build/Strategic/Map_Screen_Interface_Border.cc
  Build/Strategic/Map_Screen_Interface_Bottom.cc
  Build/Strategic/Map_Screen_Interface_Map.cc
  Build/Strategic/Map_Screen_Interface_Map_Inventory.cc
  Build/Strategic/Map_Screen_Interface_TownMine_Info.cc
  Build/Strategic/Meanwhile.cc
  Build/Strategic/Merc_Contract.cc
  Build/Strategic/Player_Command.cc
  Build/Strategic/PreBattle_Interface.cc
  Build/Strategic/Queen_Command.cc
  Build/Strategic/QuestText.cc
  Build/Strategic/Quest_Debug_System.cc
  Build/Strategic/Quests.cc
  Build/Strategic/Scheduling.cc
  Build/Strategic/Strategic.cc
  Build/Strategic/StrategicMap.cc
  Build/Strategic/Strategic_AI.cc
  Build/Strategic/Strategic_Event_Handler.cc
  Build/Strategic/Strategic_Merc_Handler.cc
  Build/Strategic/Strategic_Mines.cc
  Build/Strategic/Strategic_Movement.cc
  Build/Strategic/Strategic_Movement_Costs.cc
  Build/Strategic/Strategic_Pathing.cc
  Build/Strategic/Strategic_Status.cc
  Build/Strategic/Strategic_Town_Loyalty.cc
  Build/Strategic/Strategic_Turns.cc
  Build/Strategic/Town_Militia.cc
  Build/Sys_Globals.cc
  Build/Tactical/Air_Raid.cc
  Build/Tactical/Animation_Cache.cc
  Build/Tactical/Animation_Control.cc
  Build/Tactical/Animation_Data.cc
  Build/Tactical/ArmsDealerInvInit.cc
  Build/Tactical/Arms_Dealer_Init.cc
  Build/Tactical/Auto_Bandage.cc
  Build/Tactical/Boxing.cc
  Build/Tactical/Bullets.cc
  Build/Tactical/Campaign.cc
  Build/Tactical/Civ_Quotes.cc
  Build/Tactical/Dialogue_Control.cc
  Build/Tactical/DisplayCover.cc
  Build/Tactical/Drugs_And_Alcohol.cc
  Build/Tactical/End_Game.cc
  Build/Tactical/Enemy_Soldier_Save.cc
  Build/Tactical/FOV.cc
  Build/Tactical/Faces.cc
  Build/Tactical/Gap.cc
  Build/Tactical/Handle_Doors.cc
  Build/Tactical/Handle_Items.cc
  Build/Tactical/Handle_UI.cc
  Build/Tactical/Interface.cc
  Build/Tactical/Interface_Control.cc
  Build/Tactical/Interface_Cursors.cc
  Build/Tactical/Interface_Dialogue.cc
  Build/Tactical/Interface_Items.cc
  Build/Tactical/Interface_Panels.cc
  Build/Tactical/Interface_Utils.cc
  Build/Tactical/Inventory_Choosing.cc
  Build/Tactical/Items.cc
  Build/Tactical/Keys.cc
  Build/Tactical/LOS.cc
  Build/Tactical/LoadSaveBullet.cc
  Build/Tactical/LoadSaveMercProfile.cc
  Build/Tactical/LoadSaveObjectType.cc
  Build/Tactical/LoadSaveRottingCorpse.cc
  Build/Tactical/LoadSaveSoldierCreate.cc
  Build/Tactical/LoadSaveSoldierType.cc
  Build/Tactical/LoadSaveVehicleType.cc
  Build/Tactical/Map_Information.cc
  Build/Tactical/Merc_Entering.cc
  Build/Tactical/Merc_Hiring.cc
  Build/Tactical/Militia_Control.cc
  Build/Tactical/Morale.cc
  Build/Tactical/OppList.cc
  Build/Tactical/Overhead.cc
  Build/Tactical/PathAI.cc
  Build/Tactical/Points.cc
  Build/Tactical/QArray.cc
  Build/Tactical/Real_Time_Input.cc
  Build/Tactical/Rotting_Corpses.cc
  Build/Tactical/ShopKeeper_Interface.cc
  Build/Tactical/SkillCheck.cc
  Build/Tactical/Soldier_Add.cc
  Build/Tactical/Soldier_Ani.cc
  Build/Tactical/Soldier_Control.cc
  Build/Tactical/Soldier_Create.cc
  Build/Tactical/Soldier_Find.cc
  Build/Tactical/Soldier_Init_List.cc
  Build/Tactical/Soldier_Profile.cc
  Build/Tactical/Soldier_Tile.cc
  Build/Tactical/Spread_Burst.cc
  Build/Tactical/Squads.cc
  Build/Tactical/Strategic_Exit_GUI.cc
  Build/Tactical/Structure_Wrap.cc
  Build/Tactical/Tactical_Save.cc
  Build/Tactical/Tactical_Turns.cc
  Build/Tactical/TeamTurns.cc
  Build/Tactical/Turn_Based_Input.cc
  Build/Tactical/UI_Cursors.cc
  Build/Tactical/Vehicles.cc
  Build/Tactical/Weapons.cc
  Build/Tactical/World_Items.cc
  Build/TacticalAI/AIList.cc
  Build/TacticalAI/AIMain.cc
  Build/TacticalAI/AIUtils.cc
  Build/TacticalAI/Attacks.cc
  Build/TacticalAI/CreatureDecideAction.cc
  Build/TacticalAI/DecideAction.cc
  Build/TacticalAI/FindLocations.cc
  Build/TacticalAI/Knowledge.cc
  Build/TacticalAI/Medical.cc
  Build/TacticalAI/Movement.cc
  Build/TacticalAI/NPC.cc
  Build/TacticalAI/PanicButtons.cc
  Build/TacticalAI/Realtime.cc
  Build/TileEngine/Ambient_Control.cc
  Build/TileEngine/Buildings.cc
  Build/TileEngine/Environment.cc
  Build/TileEngine/Exit_Grids.cc
  Build/TileEngine/Explosion_Control.cc
  Build/TileEngine/Fog_Of_War.cc
  Build/TileEngine/Interactive_Tiles.cc
  Build/TileEngine/Isometric_Utils.cc
  Build/TileEngine/LightEffects.cc
  Build/TileEngine/Lighting.cc
  Build/TileEngine/LoadSaveExplosionType.cc
  Build/TileEngine/LoadSaveLightEffect.cc
  Build/TileEngine/LoadSaveLightSprite.cc
  Build/TileEngine/LoadSaveRealObject.cc
  Build/TileEngine/LoadSaveSmokeEffect.cc
  Build/TileEngine/Map_Edgepoints.cc
  Build/TileEngine/Overhead_Map.cc
  Build/TileEngine/Phys_Math.cc
  Build/TileEngine/Physics.cc
  Build/TileEngine/Pits.cc
  Build/TileEngine/Radar_Screen.cc
  Build/TileEngine/RenderWorld.cc
  Build/TileEngine/Render_Dirty.cc
  Build/TileEngine/Render_Fun.cc
  Build/TileEngine/SaveLoadMap.cc
  Build/TileEngine/Simple_Render_Utils.cc
  Build/TileEngine/Smell.cc
  Build/TileEngine/SmokeEffects.cc
  Build/TileEngine/Structure.cc
  Build/TileEngine/SysUtil.cc
  Build/TileEngine/Tactical_Placement_GUI.cc
  Build/TileEngine/TileDat.cc
  Build/TileEngine/TileDef.cc
  Build/TileEngine/Tile_Animation.cc
  Build/TileEngine/Tile_Cache.cc
  Build/TileEngine/Tile_Surface.cc
  Build/TileEngine/WorldDat.cc
  Build/TileEngine/WorldDef.cc
  Build/TileEngine/WorldMan.cc
  Build/Utils/Animated_ProgressBar.cc
  Build/Utils/Cinematics.cc
  Build/Utils/Cursors.cc
  Build/Utils/Debug_Control.cc
  Build/Utils/Encrypted_File.cc
  Build/Utils/Event_Manager.cc
  Build/Utils/Event_Pump.cc
  Build/Utils/Font_Control.cc
  Build/Utils/MapUtility.cc
  Build/Utils/MercTextBox.cc
  Build/Utils/Message.cc
  Build/Utils/Multi_Language_Graphic_Utils.cc
  Build/Utils/Music_Control.cc
  Build/Utils/PopUpBox.cc
  Build/Utils/Quantize.cc
  Build/Utils/STIConvert.cc
  Build/Utils/Slider.cc
  Build/Utils/Sound_Control.cc
  Build/Utils/Text.cc
  Build/Utils/Text_Input.cc
  Build/Utils/Text_Utils.cc
  Build/Utils/Timer_Control.cc
  Build/Utils/Utilities.cc
  Build/Utils/WordWrap.cc
  sgp/Button_Sound_Control.cc
  sgp/Button_System.cc
  sgp/Config.cc
  sgp/Container.cc
  sgp/Cursor_Control.cc
  sgp/Debug.cc
  sgp/FileMan.cc
  sgp/Font.cc
  sgp/HImage.cc
  sgp/ImpTGA.cc
  sgp/Input.cc
  sgp/LibraryDataBase.cc
  sgp/Line.cc
  sgp/MemMan.cc
  sgp/MouseSystem.cc
  sgp/PCX.cc
  sgp/Random.cc
  sgp/SGP.cc
  sgp/SGPStrings.cc
  sgp/STCI.cc
  sgp/Shading.cc
  sgp/Smack_Stub.cc
  sgp/SoundMan.cc
  sgp/TranslationTable.c
  sgp/VObject.cc
  sgp/VObject_Blitters.cc
  sgp/VSurface.cc
  sgp/Video.cc
  Build/Utils/_DutchText.cc
  Build/Utils/_EnglishText.cc
  Build/Utils/_FrenchText.cc
  Build/Utils/_GermanText.cc
  Build/Utils/_ItalianText.cc
  Build/Utils/_JA25EnglishText.cc
  Build/Utils/_JA25GermanText.cc
  Build/Utils/_JA25RussianText.cc
  Build/Utils/_PolishText.cc
  Build/Utils/_RussianText.cc
)

set( JA2EDITORSRCS
  Build/Editor/Cursor_Modes.cc
  Build/Editor/EditScreen.cc
  Build/Editor/Edit_Sys.cc
  Build/Editor/EditorBuildings.cc
  Build/Editor/EditorItems.cc
  Build/Editor/EditorMapInfo.cc
  Build/Editor/EditorMercs.cc
  Build/Editor/EditorTerrain.cc
  Build/Editor/Editor_Callbacks.cc
  Build/Editor/Editor_Modes.cc
  Build/Editor/Editor_Taskbar_Creation.cc
  Build/Editor/Editor_Taskbar_Utils.cc
  Build/Editor/Editor_Undo.cc
  Build/Editor/Item_Statistics.cc
  Build/Editor/LoadScreen.cc
  Build/Editor/MessageBox.cc
  Build/Editor/NewSmooth.cc
  Build/Editor/PopupMenu.cc
  Build/Editor/Road_Smoothing.cc
  Build/Editor/Sector_Summary.cc
  Build/Editor/SelectWin.cc
  Build/Editor/SmartMethod.cc
  Build/Editor/Smooth.cc
  Build/Editor/Smoothing_Utils.cc
)

IF(JA2EDITOR)
  set(JA2SRCS ${JA2SRCS} ${JA2EDITORSRCS})
ENDIF(JA2EDITOR)

add_executable( ja2 ${JA2SRCS})
target_link_libraries(ja2 ${SDL_LIBRARY})

IF(UNIX)
  set( INSTALLDIR /usr/local)
ELSE(UNIX)
  set( INSTALLDIR "C:\\Program Files\\ja2\\")
ENDIF(UNIX)

install(PROGRAMS ja2
  RUNTIME DESTINATION ${INSTALLDIR}/bin
  PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

