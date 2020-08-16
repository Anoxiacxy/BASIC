#pragma once
#include <memory>
#include "ASTNode.hpp"

class ControlFlowGraphNode {
public:
    std::vector<std::shared_ptr<StmtAST> >stmts;
    std::shared_ptr<ControlFlowGraphNode>target[2];
    std::vector<std::shared_ptr<Quaternion> > quaternions;
    ControlFlowGraphNode() {}
    void appendStmt(std::shared_ptr<StmtAST> stmt) {
        stmts.push_back(stmt);
    }
};