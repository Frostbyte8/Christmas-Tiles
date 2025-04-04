#pragma once

// OmniCPPComplete has an issue with lines such as:
//
// const uint8_t& width = gamePresenter->getWidth();
//
// The act of making width equal to data gamePresenter prevent gamePresenter
// from being used again. However, having "gamePresenter;" before those lines
// will prevent it from breaking. OmniCPPComplete isn't maintained, and I'm far
// from a expert in vimscript, so I am unable to fix the bug myself. This is
// basically a cheap hack to get things working again.

#define OMNICPP_DECL(X) 
