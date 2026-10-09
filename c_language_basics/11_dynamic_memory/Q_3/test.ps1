$ErrorActionPreference = 'Stop'

# 自动把输入送入 main，再关闭输入产生 EOF；核对完整字符串而非只看退出码。
function Test-StringCase {
    param([string]$Name, [string]$InputText, [string]$Expected, [bool]$ShouldFail = $false)

    $startInfo = New-Object System.Diagnostics.ProcessStartInfo
    $startInfo.FileName = Join-Path $PSScriptRoot 'main.exe'
    $startInfo.UseShellExecute = $false
    $startInfo.CreateNoWindow = $true
    $startInfo.RedirectStandardInput = $true
    $startInfo.RedirectStandardOutput = $true
    $startInfo.RedirectStandardError = $true
    $process = New-Object System.Diagnostics.Process
    $process.StartInfo = $startInfo

    try {
        [void]$process.Start()
        # 提前异步读取，长字符串输出时不会因管道缓冲区满而卡住。
        $outputTask = $process.StandardOutput.ReadToEndAsync()
        $errorTask = $process.StandardError.ReadToEndAsync()
        $process.StandardInput.Write($InputText)
        $process.StandardInput.Close()
        if (-not $process.WaitForExit(5000)) {
            $process.Kill()
            throw "$Name timed out"
        }
        $output = $outputTask.Result.Replace("`r`n", "`n")
        $errorText = $errorTask.Result

        if ($ShouldFail) {
            if ($process.ExitCode -eq 0 -or $errorText.Length -eq 0 -or $output.Length -ne 0) {
                throw "$Name did not reject invalid text"
            }
        } else {
            # 本脚本使用 ASCII 测试数据，字符数与字节数相同。
            $expectedOutput = 'Length: ' + $Expected.Length + "`nCopy: " + $Expected + "`n"
            if ($process.ExitCode -ne 0 -or $errorText.Length -ne 0 -or $output -cne $expectedOutput) {
                throw "$Name output or exit code mismatch"
            }
        }
        Write-Output "PASS: $Name"
    } finally {
        $process.Dispose()
    }
}

Test-StringCase -Name 'immediate EOF' -InputText '' -Expected ''
Test-StringCase -Name 'empty line' -InputText "`n" -Expected ''
Test-StringCase -Name 'one character then EOF' -InputText 'x' -Expected 'x'
Test-StringCase -Name 'spaces and tab' -InputText "  hello`tworld  `n" -Expected "  hello`tworld  "
Test-StringCase -Name 'stop at first newline' -InputText "first`nsecond`n" -Expected 'first'

# 覆盖“刚好留一个终止符位置”与“需要扩容”的两侧，以及多次扩容。
foreach ($length in @(15, 16, 31, 32, 100000)) {
    $text = 'a' * $length
    Test-StringCase -Name "length $length" -InputText $text -Expected $text
}
Test-StringCase -Name 'embedded NUL' -InputText ("ab" + [char]0 + "cd") -ShouldFail $true
Write-Output 'All 11 string tests passed.'
