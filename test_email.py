import smtplib
from email.mime.text import MIMEText
from datetime import datetime

def send_email():
    email = "rria4@proton.me"
    passw = "Circuit0$"
    password = passw
    sender = email
    receiver = email

    now = datetime.now()
    data = now.strftime("%d/%m/%Y")
    hora = now.strftime("%H:%M")

    corpo = f"""
        ALERTA DE SEGURANÇA

        Uma tentativa de invasão foi detectada no sistema.

        Data: {data}
        Horário: {hora}

        Foram realizadas duas tentativas consecutivas de autenticação sem sucesso.
        Recomenda-se verificar o acesso ao local monitorado.

        Este é um aviso automático do sistema de segurança.
    """

    msg = MIMEText(corpo)
    msg["Subject"] = "Alerta de Segurança - Tentativa de Invasão"
    msg["From"] = sender
    msg["To"] = receiver

    server = smtplib.SMTP("smtp.gmail.com", 587)
    server.starttls()
    server.login(sender, password)
    server.send_message(msg)
    server.quit()
