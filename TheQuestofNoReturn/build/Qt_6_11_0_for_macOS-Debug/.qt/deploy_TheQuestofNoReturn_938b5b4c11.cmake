include("/Users/janaali/cs2-Game-project/TheQuestofNoReturn/build/Qt_6_11_0_for_macOS-Debug/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/TheQuestofNoReturn-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "TheQuestofNoReturn.app"
)
