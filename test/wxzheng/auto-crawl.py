import os, sys, json, glob, shutil
import git
from pathlib import Path

# ---------- 参数 ----------
GIT_URL     = str(sys.argv[1]) if len(sys.argv) > 1 else "https://github.com/RayZh-hs/RCompiler-Testcases"
LOCAL_DIR   = sys.argv[2] if len(sys.argv) > 2 else Path(GIT_URL).stem
OUT_TC      = Path(LOCAL_DIR) / "testcases"
OUT_CPP     = Path(LOCAL_DIR) / "test.cpp"

# ---------- 1. Git ----------
if Path(LOCAL_DIR).exists():
    print(">>> 拉取最新代码…")
    repo = git.Repo(LOCAL_DIR)
    repo.remotes.origin.pull()
else:
    print(">>> 克隆仓库…")
    repo = git.Repo.clone_from(GIT_URL, LOCAL_DIR)

os.chdir(LOCAL_DIR)

# ---------- 2. 复制 *.rx → testcases/*.in
OUT_TC.mkdir(parents=True, exist_ok=True)  # parents=True 是关键
rx_files = list(Path(".").rglob("*.rx"))
seen = set()
for rx in rx_files:
    # 去重命名：用相对路径 _ 替换 /
    rel = rx.stem
    if rel in seen:
        continue
    seen.add(rel)
    dst = OUT_TC / f"{rel}.in"
    shutil.copy2(rx, dst)
print(f">>> 已复制 {len(seen)} 个 .rx → {OUT_TC}")

# ---------- 3. 生成 all_tests.cpp ----------
jsons = list(Path(".").rglob("testcase_info.json"))
blocks = []
for jf in jsons:
    data = json.loads(jf.read_text(encoding="utf8"))
    name = data.get("name", jf.parent.name)
    code = data.get("compileexitcode", 0)
    inp  = f"testcases/{name}.in"
    expect = "EXPECT_NO_THROW" if code == 0 else "EXPECT_ANY_THROW"
    blocks.append(f"TEST(Semantic, {name}) {{"
                  f"{expect}(runSemantic(\"{inp}\"));"
                  f"}}")
OUT_CPP.write_text(R"""#include "../../../../src/parser.h"
#include "../../../../src/type.h"
#include<iostream>
#include <gtest/gtest.h>

std::string openFile(std::string path)
{
    path="../../../../../test/wxzheng/RCompiler-Testcases/RCompiler-Testcases/"+path;
    freopen(path.c_str(),"r",stdin);
    int in;
    std::string code;
    while((in=std::cin.get())!=EOF)
        code.push_back(static_cast<char>(in));
    fclose(stdin);
    std::cin.clear();
    code.push_back('\n');
    return code;
}

void runSemantic(std::string path)
{
    const auto code=openFile(path);
    semanticCheckType(parser(code).solve());
}
"""
+("\n".join(blocks)), encoding="utf8")
print(f">>> 已生成 {OUT_CPP}  （共 {len(blocks)} 条 TEST）")