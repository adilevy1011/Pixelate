#include "docs.h"

#include "pxldocs_embedded.h"

namespace PixelateDocs {

const std::string& embeddedPxldocs() {
    static const std::string content = pixelate_docs::kEmbeddedPxldocs;
    return content;
}

}
