#include <iostream>
#include <fstream>
#include <utility>

#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>

using llvm::LLVMContext;
using llvm::SMDiagnostic;
using llvm::Module;
using llvm::IRBuilder;
using llvm::CallInst;
using llvm::StoreInst;
using llvm::BranchInst;
using llvm::Instruction;
using llvm::FunctionType;
using llvm::ConstantInt;
using llvm::Type;
using llvm::ArrayRef;
using llvm::ConstantDataArray;

void parse(const char* path, std::unique_ptr<Module>& program, LLVMContext& ctx)
{
    SMDiagnostic error;
    
    program = llvm::parseIRFile(path, error, ctx);
    if (!program)
    {
        printf("Failed to parse IR file\n");
        error.print(path, llvm::errs());

        exit(-1);
    }
}

void dump(const char* path, std::unique_ptr<Module>& program)
{
    std::string ir;
    llvm::raw_string_ostream stream(ir);
    program->print(stream, nullptr);

    std::ofstream output(path);
    output << ir;
    output.close();
}

Instruction* find_store(Instruction* start, const char* target)
{
    auto previous_instruction = start->getPrevNode();

    while (previous_instruction != nullptr)
    {
        // we only want to check store instructions
        if (llvm::isa<StoreInst>(previous_instruction))
        {
            const auto store_instruction = llvm::cast<StoreInst>(previous_instruction);
            const auto target_operand = store_instruction->getOperand(1);
            const auto operand_name = target_operand->getName().data();

            // make sure the operand (register) to be written matches our target
            if (strcmp(operand_name, target) == 0)
                return previous_instruction;
        }

        previous_instruction = previous_instruction->getPrevNode();
    }

    return nullptr;
}

void process(const std::unique_ptr<Module>& program, IRBuilder<>& builder)
{
    const auto function_call = program->getFunction("function_call");
    const auto fmt_str = builder.CreateGlobalStringPtr("dlsym => %p\n", "dlsym_fmt", 0, program.get());
    const auto print = program->getFunction("printf");
    const auto print_checked = program->getFunction("print_checked");

    for (const auto& user : function_call->users())
    {
        // make sure the reference is actually a call instruction
        if (!llvm::isa<CallInst>(user))
            continue;

        const auto call_instruction = llvm::cast<CallInst>(user);
        const auto store_instruction = find_store(call_instruction, "rsi");
        if (store_instruction == nullptr)
            continue;

        const auto rsi = store_instruction->getOperand(1);

        // we want to emit instructions after the store instruction
        builder.SetInsertPoint(store_instruction->getNextNode());
        const auto loaded = builder.CreateLoad(rsi);
        builder.CreateCall(print, { fmt_str, loaded });

        // emit a call to an external checked printf
        const auto ptr_type = Type::getIntNPtrTy(program->getContext(), 8);
        const auto ptr = builder.CreateCast(Instruction::CastOps::IntToPtr, loaded, ptr_type);
        builder.CreateCall(print_checked, { ptr });
    }
}

void create_printf(const std::unique_ptr<Module>& program, IRBuilder<>& builder)
{
    std::vector<Type*> args = { builder.getInt8Ty()->getPointerTo(), builder.getInt64Ty() };
    auto function_type = FunctionType::get(builder.getInt64Ty(), args, false);

    program->getOrInsertFunction("printf", function_type);
}

void create_print_checked(const std::unique_ptr<Module>& program, IRBuilder<>& builder)
{
    std::vector<Type*> args = { builder.getInt8Ty()->getPointerTo() };
    auto function_type = FunctionType::get(builder.getVoidTy(), args, false);

    program->getOrInsertFunction("print_checked", function_type);
}

int main(int argc, char* argv[])
{
    LLVMContext context;
    std::unique_ptr<Module> program = nullptr;
    parse(argv[1], program, context);

    printf("Loaded IR: %s\n", program->getModuleIdentifier().data());

    IRBuilder builder(context);

    create_printf(program, builder);
    create_print_checked(program, builder);
    process(program, builder);

    printf("Verification: %d\n", llvm::verifyModule(*program, &llvm::dbgs()));
    dump(argv[2], program);
 
    return 0;
}
