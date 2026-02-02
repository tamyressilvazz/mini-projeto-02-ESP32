import socket
import sys
from datetime import datetime
from test_email import send_email

HOST = ''
PORT = 1408
USERS_FILE = "users.txt"

host_name = socket.gethostname()
host_ip = socket.gethostbyname(host_name)

failed_attempts = 0

def load_users():
    users = {}
    with open(USERS_FILE, "r") as f:
        for line in f:
            if ":" in line:
                user, pwd = line.strip().split(":")
                users[pwd] = user
    return users

mysock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    mysock.bind((HOST, PORT))
except socket.error:
    print("Failed to bind")
    sys.exit()

mysock.listen(5)
print(f"Servidor rodando em: {host_ip}...")

while True:
    conn, addr = mysock.accept()
    data = conn.recv(1024)

    if not data:
        conn.close()
        continue

    request = data.decode("utf-8")
    print("Requisição recebida:", request)

    response_body = "ERRO"
    users = load_users()

    if "GET /auth?password=" in request:
        password = request.split("password=")[1].split(" ")[0]

        if password in users:
            response_body = "USER:" + users[password]
            print(response_body)
            failed_attempts = 0  # zera falhas após sucesso
        else:
            failed_attempts += 1
            print(f"Falha consecutiva: {failed_attempts}")

            if failed_attempts >= 2:
                send_email()
                failed_attempts = 0  # evita spam de emails

    elif "GET /test_connection" in request:
        response_body = "OK"

    response = (
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: close\r\n\r\n"
        + response_body
    )

    conn.sendall(response.encode())
    conn.close()
