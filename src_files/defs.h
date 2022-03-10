//
// Created by Luecx on 10.03.2022.
//

#ifndef KOIVISTO_DEFS_H
#define KOIVISTO_DEFS_H

// this file checks for what features should be compiled into
// the engine. Normally the flags for TABLEBASES should be enabled.
// If they are disabled, they will not be usable by the engine.

// emscripten requires c files to be disabled
#ifdef __EMSCRIPTEN__
#undef SUPPORT_TABLEBASES
#endif

#endif    // KOIVISTO_DEFS_H
