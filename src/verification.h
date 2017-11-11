
#include "ast.h"
#include "resolver.h"
#include "workspace.h"

class Verification {
private:
    WorkSpace workspace;
public:
    inline Verification(const WorkSpace& workspace) : workspace(workspace) {}
    const bool validate(const std::vector<AST_Type*>& contained);
};
