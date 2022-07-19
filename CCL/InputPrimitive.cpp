#include "CCL.h"

#include "InputPrimitive.h"


void KeyboardManager::Init() {
    InputData data[] = {
        { CCL_K_UP      , 'W'           },
        { CCL_K_DOWN    , 'S'           },
        { CCL_K_RIGHT   , 'D'           },
        { CCL_K_LEFT    , 'A'           },
        { CCL_K_A       , 'Z'           },
        { CCL_K_B       , 'X'           },
        { CCL_K_X       , 'Q'           },
        { CCL_K_Y       , 'E'           },
        { CCL_K_RTRG1   , VK_SPACE      },
        { CCL_K_RTRG2   , VK_LSHIFT     },
        { CCL_K_LTRG1   , VK_LMENU      },
        { CCL_K_LTRG2   , VK_TAB        },
        { CCL_K_RSTICK  , VK_RBUTTON    },
        { CCL_K_LSTICK  , VK_LBUTTON    },
        { CCL_K_START   , VK_SELECT     },
        { CCL_K_PAUSE   , VK_ESCAPE     },
        { CCL_K_HOME    , VK_BACK       },
    };

    LoadInputData(data, sizeof(data) / sizeof(InputData));
}

POINT   Cursor::screen = {};
POINT   Cursor::client = {};
POINT   Cursor::log = {};
POINT   Cursor::delta = {};
int     Cursor::showCounta = 0;