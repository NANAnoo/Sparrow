set(ImGuizmoSourceDir ${ExternalDir}/imGuizmo)

set(ImGuizmoSrc
        "${ImGuizmoSourceDir}/GraphEditor.cpp"
        "${ImGuizmoSourceDir}/GraphEditor.h"
        "${ImGuizmoSourceDir}/ImCurveEdit.cpp"
        "${ImGuizmoSourceDir}/ImCurveEdit.h"
        "${ImGuizmoSourceDir}/ImGradient.h"
        "${ImGuizmoSourceDir}/ImGradient.cpp"
        "${ImGuizmoSourceDir}/ImGuizmo.cpp"
        "${ImGuizmoSourceDir}/ImGuizmo.h"
        "${ImGuizmoSourceDir}/ImSequencer.h"
        "${ImGuizmoSourceDir}/ImSequencer.cpp"
        "${ImGuizmoSourceDir}/ImZoomSlider.h"
        )

add_library(ImGuizmo STATIC ${ImGuizmoSrc})
target_include_directories(ImGuizmo PUBLIC "${ImGuizmoSourceDir}")

