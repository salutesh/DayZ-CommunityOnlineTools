function Get-ScriptDirectory {
    Split-Path -Parent $PSCommandPath
}

$path = Get-ScriptDirectory

$username = Get-Content -Path ($path + "..\..\..\Keys\username.txt")
$password = Get-Content -Path ($path + "..\..\..\Keys\password.txt")

$secure_password = ConvertTo-SecureString -String $password -AsPlainText -Force

$cred = New-Object -TypeName System.Management.Automation.PSCredential -ArgumentList $username, $secure_password

Invoke-Command -ComputerName "DESKTOP-TMCUVSS" -Credential $cred -ScriptBlock { 
    Invoke-Expression -Command:"cmd.exe /c `"C:\Sharing\ExitServer.bat`""
}