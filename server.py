import http.server
import socketserver
import json
import os
import subprocess
import tempfile
import webbrowser
import re
import shutil

PORT = 5000
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
WEB_DIR = os.path.join(BASE_DIR, "frontend")
COMPILER_BIN = os.path.join(BASE_DIR, "compiler.exe")
C_COMPILER_BIN = os.path.join(BASE_DIR, "C_Compiler", "c_compiler.exe")
CPP_COMPILER_BIN = os.path.join(BASE_DIR, "CPP_Compiler", "cpp_compiler.exe")
JAVA_COMPILER_BIN = os.path.join(BASE_DIR, "Java_Compiler", "java_compiler.exe")

class CompilerHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=WEB_DIR, **kwargs)

    def do_GET(self):
        if self.path == "/" or self.path == "":
            self.path = "/index.html"
        return super().do_GET()

    def do_POST(self):
        if self.path == "/compile":
            content_length = int(self.headers.get("Content-Length", 0))
            body = self.rfile.read(content_length).decode("utf-8")
            
            try:
                data = json.loads(body)
                lang = data.get("lang", "c")
                code = data.get("code", "")

                ext_map = {"c": ".c", "cpp": ".cpp", "java": ".java", "mc": ".mc"}
                ext = ext_map.get(lang, ".c")

                # Create a isolated build directory for this compilation job
                temp_dir = tempfile.mkdtemp(prefix="build_", dir=BASE_DIR)
                if lang == "java":
                    m = re.search(r'class\s+([A-Za-z_][A-Za-z0-9_]*)', code)
                    classname = m.group(1) if m else "Main"
                    tmp_filename = f"{classname}.java"
                elif lang == "cpp":
                    tmp_filename = "main.cpp"
                else:
                    tmp_filename = "main.c"

                tmp_path = os.path.join(temp_dir, tmp_filename)
                with open(tmp_path, "w", encoding="utf-8") as f:
                    f.write(code)

                # Route to dedicated subcompiler or fallback to unified driver
                cmd = [COMPILER_BIN, f"--lang={lang}"]
                if lang == "c" and os.path.exists(C_COMPILER_BIN):
                    cmd = [C_COMPILER_BIN]
                elif lang == "cpp" and os.path.exists(CPP_COMPILER_BIN):
                    cmd = [CPP_COMPILER_BIN]
                elif lang == "java" and os.path.exists(JAVA_COMPILER_BIN):
                    cmd = [JAVA_COMPILER_BIN]
                cmd.append(tmp_path)
                proc = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

                # Target Program Execution
                program_output = ""
                execution_failed = False
                try:
                    if lang == "c":
                        bin_out = os.path.join(temp_dir, "main.exe")
                        comp = subprocess.run(["gcc", "-O0", tmp_path, "-o", bin_out], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                        if comp.returncode == 0:
                            run = subprocess.run([bin_out], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, timeout=5)
                            program_output = run.stdout + ("\n" + run.stderr if run.stderr else "")
                        else:
                            program_output = "[GCC Build Error]\n" + (comp.stderr or comp.stdout)
                            execution_failed = True
                    elif lang == "cpp":
                        bin_out = os.path.join(temp_dir, "main.exe")
                        comp = subprocess.run(["g++", "-O0", tmp_path, "-o", bin_out], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                        if comp.returncode == 0:
                            run = subprocess.run([bin_out], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, timeout=5)
                            program_output = run.stdout + ("\n" + run.stderr if run.stderr else "")
                        else:
                            program_output = "[G++ Build Error]\n" + (comp.stderr or comp.stdout)
                            execution_failed = True
                    elif lang == "java":
                        comp = subprocess.run(["javac", tmp_filename], cwd=temp_dir, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                        if comp.returncode == 0:
                            run = subprocess.run(["java", "-cp", ".", classname], cwd=temp_dir, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, timeout=5)
                            program_output = run.stdout + ("\n" + run.stderr if run.stderr else "")
                        else:
                            program_output = "[Javac Build Error]\n" + (comp.stderr or comp.stdout)
                            execution_failed = True
                except Exception as ex:
                    program_output = f"[Execution Error] {str(ex)}"
                    execution_failed = True

                # Clean up isolated build directory
                try:
                    shutil.rmtree(temp_dir, ignore_errors=True)
                except Exception:
                    pass

                stdout = proc.stdout
                stderr = proc.stderr
                # Strip out internal temp source file path from terminal logs
                stdout = re.sub(r'Source File: [^\n]+\n*', '', stdout)
                success = (proc.returncode == 0) and not execution_failed

                # Parse all 6 phases from compiler stdout
                phase1_tokens = ""
                phase2_ast = ""
                phase3_symtab = ""
                phase4_tac = ""
                phase5_opt = ""
                phase6_codegen = ""

                if "PHASE 1: LEXICAL ANALYSIS" in stdout:
                    p1 = stdout.split("PHASE 1: LEXICAL ANALYSIS")[1].split("PHASE 2: SYNTAX ANALYSIS")[0]
                    phase1_tokens = p1.strip().strip("=").strip()

                if "PHASE 2: SYNTAX ANALYSIS" in stdout:
                    p2 = stdout.split("PHASE 2: SYNTAX ANALYSIS")[1].split("PHASE 3: SEMANTIC ANALYSIS")[0]
                    phase2_ast = p2.strip().strip("=").strip()

                if "PHASE 3: SEMANTIC ANALYSIS" in stdout:
                    p3 = stdout.split("PHASE 3: SEMANTIC ANALYSIS")[1].split("PHASE 4: INTERMEDIATE CODE GENERATION")[0]
                    phase3_symtab = p3.strip().strip("=").strip()

                if "PHASE 4: INTERMEDIATE CODE GENERATION" in stdout:
                    p4 = stdout.split("PHASE 4: INTERMEDIATE CODE GENERATION")[1].split("PHASE 5: CODE OPTIMIZATION")[0]
                    phase4_tac = p4.strip().strip("=").strip()

                if "PHASE 5: CODE OPTIMIZATION" in stdout:
                    p5 = stdout.split("PHASE 5: CODE OPTIMIZATION")[1].split("PHASE 6: CODE GENERATION")[0]
                    phase5_opt = p5.strip().strip("=").strip()

                if "PHASE 6: CODE GENERATION" in stdout:
                    p6 = stdout.split("PHASE 6: CODE GENERATION")[1]
                    phase6_codegen = p6.strip().strip("=").strip()

                errors_text = stderr.strip()
                if not success and not errors_text:
                    if execution_failed:
                        errors_text = program_output.strip()
                    else:
                        errors_text = stdout.strip()

                response = {
                    "success": success,
                    "program_output": program_output.strip() or "// Program executed with 0 console output",
                    "phase1_tokens": phase1_tokens or "// Tokens unavailable",
                    "phase2_ast": phase2_ast or "// AST unavailable",
                    "phase3_symtab": phase3_symtab or "// Symbol Table unavailable",
                    "phase4_tac": phase4_tac or "// TAC unavailable",
                    "phase5_opt": phase5_opt or "// Optimization pass unavailable",
                    "phase6_codegen": phase6_codegen or "// Code generation unavailable",
                    "raw": stdout + "\n" + stderr,
                    "errors": errors_text
                }

                self.send_response(200)
                self.send_header("Content-Type", "application/json")
                self.end_headers()
                self.wfile.write(json.dumps(response).encode("utf-8"))

            except Exception as e:
                self.send_response(500)
                self.send_header("Content-Type", "application/json")
                self.end_headers()
                self.wfile.write(json.dumps({"success": False, "errors": str(e)}).encode("utf-8"))
        else:
            self.send_error(404, "Endpoint not found")

def run_server():
    if not os.path.exists(COMPILER_BIN):
        print(f"[!] Warning: {COMPILER_BIN} not found. Building now with make...")
        subprocess.run(["make"], cwd=BASE_DIR)

    # Allow port reuse immediately
    socketserver.TCPServer.allow_reuse_address = True
    with socketserver.TCPServer(("", PORT), CompilerHandler) as httpd:
        url = f"http://localhost:{PORT}"
        print("=" * 60)
        print("     MULTI-LANGUAGE COMPILER (C, C++, Java) - WEB IDE")
        print("=" * 60)
        print(f"[+] Server started at: {url}")
        print("[+] Opening your web browser automatically...")
        print("[+] Press Ctrl+C in this terminal to stop the server.")
        print("=" * 60)
        
        webbrowser.open(url)
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\n[+] Server stopped successfully.")

if __name__ == "__main__":
    run_server()
