import os, sys, json, glob, shutil
import git
from pathlib import Path

# ---------- 参数 ----------
GIT_URL     = str(sys.argv[1]) if len(sys.argv) > 1 else "https://github.com/RayZh-hs/RCompiler-Testcases"
LOCAL_DIR   = sys.argv[2] if len(sys.argv) > 2 else Path(GIT_URL).stem
REPO_ROOT   = Path(LOCAL_DIR).resolve()          # .../repo
OUT_BASE    = REPO_ROOT.parent     # .../generated
OUT_TC      = OUT_BASE / "testcases"
OUT_CPP     = OUT_BASE / "test.cpp"

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
blocks = {}
for jf in Path(".").rglob("testcase_info.json"):
    data = json.loads(jf.read_text(encoding="utf8"))

    rel_path = jf.parent.relative_to(".")          # a/b
    suite = rel_path.parts[0].replace("-", "")     # semantic-1 -> semantic1
    case  = rel_path.name
    code  = data.get("compileexitcode", 0)
    inp   = f"{case}.in"
    expect = "EXPECT_NO_THROW" if code == 0 else "EXPECT_ANY_THROW"

    blocks[jf] = f"TEST({suite}, {case}) {{\n" \
                 f"    {expect}(runSemantic(\"{inp}\"));\n" \
                 f"}}"
OUT_CPP.write_text(R"""#include "../../src/parser.h"
#include "../../src/type.h"
#include<iostream>
#include <gtest/gtest.h>

std::string openFile(std::string path)
{
    path="../../../test/wxzheng/testcases/"+path;
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
+("\n".join(blocks.values())), encoding="utf8")
print(f">>> 已生成 {OUT_CPP}  （共 {len(blocks)} 条 TEST）")
os.chdir(REPO_ROOT.parent)        # 必须先退出仓库目录，否则 Windows 会拒删

import os, stat, shutil, time

def force_rmtree(path):
    """Windows 专用：去只读 + 强制删除"""
    def remove_readonly(func, p, excinfo):
        os.chmod(p, stat.S_IWRITE)   # 去掉只读
        func(p)                      # 再删
    for _ in range(3):               # 最多重试 3 次
        try:
            shutil.rmtree(path, onexc=remove_readonly)
            return
        except PermissionError:
            time.sleep(0.5)
    raise RuntimeError(f"仍无法删除 {path}")

# ===== 使用 =====
os.chdir(REPO_ROOT.parent)
force_rmtree(REPO_ROOT)
print(f">>> 已删除临时仓库：{REPO_ROOT}")