Write-Host "Repairing Windows component store..." -ForegroundColor Cyan
Dism.exe /Online /Cleanup-Image /RestoreHealth

Write-Host "Running system file checker..." -ForegroundColor Cyan
sfc.exe /scannow

Write-Host "Trying to reinstall Microsoft Store..." -ForegroundColor Cyan
WSReset.exe -i

Write-Host "Re-registering Store-related packages if present..." -ForegroundColor Cyan
Get-AppxPackage -AllUsers Microsoft.WindowsStore | ForEach-Object {
    Add-AppxPackage -DisableDevelopmentMode -Register "$($_.InstallLocation)\AppXManifest.xml"
}
Get-AppxPackage -AllUsers Microsoft.StorePurchaseApp | ForEach-Object {
    Add-AppxPackage -DisableDevelopmentMode -Register "$($_.InstallLocation)\AppXManifest.xml"
}
Get-AppxPackage -AllUsers Microsoft.DesktopAppInstaller | ForEach-Object {
    Add-AppxPackage -DisableDevelopmentMode -Register "$($_.InstallLocation)\AppXManifest.xml"
}

Write-Host ""
Write-Host "Done. Restart Windows, then try installing Codex again from:" -ForegroundColor Green
Write-Host "https://apps.microsoft.com/detail/9PLM9XGG6VKS"
Read-Host "Press Enter to close"
