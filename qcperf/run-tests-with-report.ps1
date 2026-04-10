# QcPerf Testing Framework - Run Tests with Reporting (PowerShell)
# This script runs the already-built tests and generates reports
# Use build-tests.ps1 to build first, then run this to generate reports

param(
    [switch]$GenerateReport = $false,
    [string]$OutputFormat = "text"
)

# Get script directory
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$buildDir = Join-Path $scriptDir "build"
$testResultsDir = Join-Path $scriptDir "test-results"

# Create test results directory if it doesn't exist
if (-not (Test-Path $testResultsDir)) {
    New-Item -ItemType Directory -Path $testResultsDir | Out-Null
}

# Generate timestamp for results
$timestamp = Get-Date -Format 'yyyy-MM-dd-HHmmss'
$logFile = Join-Path $testResultsDir "test-run-$timestamp.log"
$jsonFile = Join-Path $testResultsDir "test-results-$timestamp.json"

# Initialize test results object
$testResults = @{
    timestamp     = $timestamp
    testStatus    = "UNKNOWN"
    suites        = @()
    totalTests    = 0
    passedTests   = 0
    failedTests   = 0
    ignoredTests  = 0
}

function Log-Message {
    param([string]$Message, [string]$Level = "INFO")
    $logEntry = "[$((Get-Date).ToString('HH:mm:ss'))] [$Level] $Message"
    Write-Host $logEntry
    Add-Content -Path $logFile -Value $logEntry
}

# Runs a single Unity test executable, parses output and returns a suite result object
function Invoke-TestSuite {
    param([string]$SuiteName, [string]$ExePath)

    $suite = @{
        name      = $SuiteName
        status    = "NOT_FOUND"
        total     = 0
        passed    = 0
        failed    = 0
        ignored   = 0
        testCases = @()
    }

    if (-not (Test-Path $ExePath)) {
        Log-Message "Executable NOT found: $ExePath" "ERROR"
        Log-Message "Did you run build-tests.ps1 first?" "INFO"
        return $suite
    }

    Log-Message "Running suite '$SuiteName': $ExePath" "SUCCESS"
    $output = & $ExePath 2>&1
    $outputStr = $output | Out-String

    Add-Content -Path $logFile -Value "`n=== $SuiteName OUTPUT ==="
    Add-Content -Path $logFile -Value $outputStr

    $output | ForEach-Object { Log-Message "$_" "TEST" }

    # Parse individual test case lines
    # Unity format: <file>:<line>:<test_name>:PASS  or  <file>:<line>:<test_name>:FAIL:<message>
    foreach ($line in $output) {
        if ($line -match '^.+:\d+:(\w+):(PASS|FAIL|IGNORE)(.*)$') {
            $suite.testCases += @{
                name    = $Matches[1]
                result  = $Matches[2]
                message = $Matches[3].TrimStart(':')
            }
        }
    }

    # Parse summary line: "5 Tests 0 Failures 0 Ignored"
    if ($outputStr -match '(\d+)\s+Tests\s+(\d+)\s+Failures\s+(\d+)\s+Ignored') {
        $suite.total   = [int]$Matches[1]
        $suite.failed  = [int]$Matches[2]
        $suite.ignored = [int]$Matches[3]
        $suite.passed  = $suite.total - $suite.failed - $suite.ignored
    }

    $suite.status = if ($suite.total -gt 0 -and $suite.failed -eq 0) { "PASSED" } else { "FAILED" }
    return $suite
}

# Start logging
Log-Message "========================================" "INFO"
Log-Message "QcPerf Testing Framework - Run Tests with Reporting" "INFO"
Log-Message "========================================" "INFO"
Log-Message "Log file: $logFile" "INFO"

$suites = @()

try {
    Log-Message "" "INFO"
    Log-Message "========================================" "INFO"
    Log-Message "Running Unit Tests..." "INFO"
    Log-Message "========================================" "INFO"

    # Run core tests
    $coreExe = Join-Path $buildDir "test_app\Release\test_qcperf_core.exe"
    $coreSuite = Invoke-TestSuite -SuiteName "qcperf_core" -ExePath $coreExe
    $suites += $coreSuite

    Log-Message "" "INFO"

    # Run backends tests
    $backendsExe = Join-Path $buildDir "test_app\Release\test_qcperf_backends.exe"
    $backendsSuite = Invoke-TestSuite -SuiteName "qcperf_backends" -ExePath $backendsExe
    $suites += $backendsSuite

    # Aggregate totals across all suites
    foreach ($suite in $suites) {
        $testResults.totalTests   += $suite.total
        $testResults.passedTests  += $suite.passed
        $testResults.failedTests  += $suite.failed
        $testResults.ignoredTests += $suite.ignored
    }
    $testResults.suites = $suites

    $anyFailed = $suites | Where-Object { $_.status -ne "PASSED" }
    if ($anyFailed) {
        $testResults.testStatus = "FAILED"
        Log-Message "" "INFO"
        Log-Message "========================================" "INFO"
        Log-Message "SOME TESTS FAILED" "ERROR"
        Log-Message "========================================" "INFO"
    } else {
        $testResults.testStatus = "PASSED"
        Log-Message "" "INFO"
        Log-Message "========================================" "INFO"
        Log-Message "ALL TESTS PASSED" "SUCCESS"
        Log-Message "========================================" "INFO"
    }

} catch {
    Log-Message "Error: $_" "ERROR"
    $testResults.testStatus = "ERROR"
}

# Generate summary
Log-Message "" "INFO"
Log-Message "========================================" "INFO"
Log-Message "TEST SUMMARY" "INFO"
Log-Message "========================================" "INFO"
Log-Message "Test Status:   $($testResults.testStatus)" "INFO"
Log-Message "Total Tests:   $($testResults.totalTests)" "INFO"
Log-Message "Passed Tests:  $($testResults.passedTests)" "INFO"
Log-Message "Failed Tests:  $($testResults.failedTests)" "INFO"
Log-Message "Ignored Tests: $($testResults.ignoredTests)" "INFO"
Log-Message "========================================" "INFO"
Log-Message "Log file saved to: $logFile" "INFO"

# Generate JSON report if requested
if ($GenerateReport -or $OutputFormat -eq "json") {
    Log-Message "Generating JSON report..." "INFO"
    $testResults | ConvertTo-Json -Depth 5 | Out-File $jsonFile -Encoding UTF8
    Log-Message "JSON report saved to: $jsonFile" "SUCCESS"
}

# Always generate HTML report
$htmlFile = Join-Path $testResultsDir "test-report-$timestamp.html"
Log-Message "Generating HTML report..." "INFO"

$overallClass = if ($testResults.testStatus -eq "PASSED") { "passed" } else { "failed" }
$overallLabel = if ($testResults.testStatus -eq "PASSED") { "ALL TESTS PASSED" } else { "SOME TESTS FAILED" }

# Build suite summary rows
$suiteRowsHtml = ""
foreach ($suite in $suites) {
    $sc = if ($suite.status -eq "PASSED") { "pass" } else { "fail" }
    $suiteRowsHtml += @"

            <tr class="$sc">
                <td>$($suite.name)</td>
                <td>$($suite.total)</td>
                <td class="num-pass">$($suite.passed)</td>
                <td class="num-fail">$($suite.failed)</td>
                <td>$($suite.ignored)</td>
                <td><span class="badge $sc">$($suite.status)</span></td>
            </tr>
"@
}

# Build per-suite test case detail sections
$testDetailsHtml = ""
foreach ($suite in $suites) {
    $suiteStatusClass = if ($suite.status -eq "PASSED") { "pass" } else { "fail" }

    $caseRowsHtml = ""
    if ($suite.testCases.Count -gt 0) {
        foreach ($tc in $suite.testCases) {
            $tcClass = $tc.result.ToLower()
            $caseRowsHtml += @"

                    <tr class="$tcClass">
                        <td>$($tc.name)</td>
                        <td><span class="badge $tcClass">$($tc.result)</span></td>
                        <td>$($tc.message)</td>
                    </tr>
"@
        }
    } else {
        $caseRowsHtml = "<tr><td colspan='3' style='color:#999;'>No test case details available (executable not found)</td></tr>"
    }

    $testDetailsHtml += @"

        <div class="suite-section">
            <h3>$($suite.name) &nbsp;<span class="badge $suiteStatusClass">$($suite.status)</span></h3>
            <table class="test-table">
                <thead><tr><th>Test Name</th><th>Result</th><th>Message</th></tr></thead>
                <tbody>$caseRowsHtml
                </tbody>
            </table>
        </div>
"@
}

$htmlContent = @"
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>QcPerf Test Report - $timestamp</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body { font-family: 'Segoe UI', Arial, sans-serif; background: #f0f2f5; padding: 24px; }
        .container { max-width: 980px; margin: 0 auto; background: white; border-radius: 8px; box-shadow: 0 2px 8px rgba(0,0,0,0.12); overflow: hidden; }
        .header { background: #1a252f; color: white; padding: 24px 32px; }
        .header h1 { font-size: 22px; margin-bottom: 4px; }
        .header p { font-size: 13px; opacity: 0.7; }
        .body { padding: 24px 32px; }
        .summary { display: grid; grid-template-columns: repeat(4, 1fr); gap: 16px; margin-bottom: 24px; }
        .stat-box { background: #f8f9fa; border-radius: 6px; padding: 16px; border-top: 4px solid #3498db; text-align: center; }
        .stat-box.pass-box { border-top-color: #27ae60; }
        .stat-box.fail-box { border-top-color: #e74c3c; }
        .stat-box.ign-box  { border-top-color: #95a5a6; }
        .stat-label { font-size: 11px; color: #7f8c8d; text-transform: uppercase; letter-spacing: 0.5px; margin-bottom: 8px; }
        .stat-value { font-size: 32px; font-weight: 700; color: #2c3e50; }
        .status-banner { padding: 12px 20px; border-radius: 6px; font-weight: 600; margin-bottom: 24px; font-size: 15px; }
        .status-banner.passed { background: #d4edda; color: #155724; border: 1px solid #c3e6cb; }
        .status-banner.failed { background: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; }
        h2 { font-size: 15px; font-weight: 600; margin-bottom: 12px; color: #2c3e50; border-bottom: 2px solid #e9ecef; padding-bottom: 6px; }
        h3 { font-size: 14px; font-weight: 600; margin-bottom: 10px; color: #2c3e50; }
        table { width: 100%; border-collapse: collapse; margin-bottom: 8px; font-size: 13px; }
        th { background: #f8f9fa; padding: 10px 12px; text-align: left; font-weight: 600; color: #5a6a72; border-bottom: 2px solid #dee2e6; }
        td { padding: 9px 12px; border-bottom: 1px solid #f0f0f0; vertical-align: top; }
        tr.pass td { background: #f6fff6; }
        tr.fail td { background: #fff6f6; }
        .badge { display: inline-block; padding: 2px 10px; border-radius: 12px; font-size: 11px; font-weight: 600; white-space: nowrap; }
        .badge.passed, .badge.pass { background: #d4edda; color: #155724; }
        .badge.failed, .badge.fail { background: #f8d7da; color: #721c24; }
        .badge.ignore { background: #fff3cd; color: #856404; }
        .num-pass { color: #27ae60; font-weight: 600; }
        .num-fail { color: #e74c3c; font-weight: 600; }
        .suite-section { margin-bottom: 28px; }
        .suite-section h3 { padding-bottom: 8px; border-bottom: 1px solid #e9ecef; margin-bottom: 12px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>QcPerf Test Report</h1>
            <p>Generated: $timestamp</p>
        </div>
        <div class="body">

            <div class="summary">
                <div class="stat-box">
                    <div class="stat-label">Total Tests</div>
                    <div class="stat-value">$($testResults.totalTests)</div>
                </div>
                <div class="stat-box pass-box">
                    <div class="stat-label">Passed</div>
                    <div class="stat-value">$($testResults.passedTests)</div>
                </div>
                <div class="stat-box fail-box">
                    <div class="stat-label">Failed</div>
                    <div class="stat-value">$($testResults.failedTests)</div>
                </div>
                <div class="stat-box ign-box">
                    <div class="stat-label">Ignored</div>
                    <div class="stat-value">$($testResults.ignoredTests)</div>
                </div>
            </div>

            <div class="status-banner $overallClass">$overallLabel</div>

            <h2>Suite Summary</h2>
            <table>
                <thead>
                    <tr><th>Suite</th><th>Total</th><th>Passed</th><th>Failed</th><th>Ignored</th><th>Status</th></tr>
                </thead>
                <tbody>$suiteRowsHtml
                </tbody>
            </table>

            <h2>Test Details</h2>
$testDetailsHtml

        </div>
    </div>
</body>
</html>
"@

$htmlContent | Out-File $htmlFile -Encoding UTF8
Log-Message "HTML report saved to: $htmlFile" "SUCCESS"

try {
    Start-Process $htmlFile
    Log-Message "Opened HTML report in browser" "INFO"
} catch {
    Log-Message "Could not open HTML report in browser (but file was created)" "INFO"
}

Log-Message "" "INFO"
Log-Message "Test run completed!" "INFO"

# Exit with appropriate code
if ($testResults.testStatus -eq "PASSED") {
    exit 0
} else {
    exit 1
}
