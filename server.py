import http.server
import socketserver
import json
import os
import subprocess
import tempfile
import webbrowser

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

                # Write code to a temporary file
                with tempfile.NamedTemporaryFile("w", suffix=ext, delete=False, dir=BASE_DIR) as tmp:
                    tmp.write(code)
                    tmp_path = tmp.name

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

                # Clean up temp file
                if os.path.exists(tmp_path):
                    os.remove(tmp_path)

                stdout = proc.stdout
                stderr = proc.stderr
                success = (proc.returncode == 0)

                # Parse sections from stdout
                ast_text = ""
                symtab_text = ""
                tac_text = ""

                if "ABSTRACT SYNTAX TREE (AST)" in stdout:
                    parts = stdout.split("ABSTRACT SYNTAX TREE (AST)")
                    if len(parts) > 1:
                        sub = parts[1].split("RUNNING SEMANTIC ANALYSIS")[0]
                        ast_text = sub.strip().strip("=").strip()

                if "SYMBOL TABLE SNAPSHOT" in stdout:
                    parts = stdout.split("SYMBOL TABLE SNAPSHOT")
                    if len(parts) > 1:
                        sub = parts[1].split("Validation Succeeded")[0]
                        symtab_text = sub.strip().strip("=").strip()

                if "THREE ADDRESS CODE (TAC) OUTPUT" in stdout:
                    parts = stdout.split("THREE ADDRESS CODE (TAC) OUTPUT")
                    if len(parts) > 1:
                        sub = parts[1].strip().strip("=").strip()
                        tac_text = sub

                errors_text = stderr.strip()
                if not success and not errors_text:
                    errors_text = stdout.strip()

                response = {
                    "success": success,
                    "raw": stdout + "\n" + stderr,
                    "ast": ast_text,
                    "symtab": symtab_text,
                    "tac": tac_text,
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
