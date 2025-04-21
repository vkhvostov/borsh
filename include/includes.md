<h2 data-start="157" data-end="199">🟢 User Input &amp; Line Editing (Readline)</h2>
<p data-start="200" data-end="248">These are used to handle user input and history.</p>
<ul data-start="250" data-end="785">
<li data-start="250" data-end="346"><strong data-start="252" data-end="274"><code data-start="254" data-end="272">readline(prompt)</code></strong> – Reads a line of input from the user with editing and history support.</li>
<li data-start="347" data-end="431"><strong data-start="349" data-end="372"><code data-start="351" data-end="370">add_history(line)</code></strong> – Adds the input line to the shell history (if not empty).</li>
<li data-start="432" data-end="509"><strong data-start="434" data-end="458"><code data-start="436" data-end="456">rl_clear_history()</code></strong> – Clears the history (useful for exit or cleanup).</li>
<li data-start="510" data-end="587"><strong data-start="512" data-end="534"><code data-start="514" data-end="532">rl_on_new_line()</code></strong> – Prepares the internal state for a new prompt line.</li>
<li data-start="588" data-end="698"><strong data-start="590" data-end="629"><code data-start="592" data-end="627">rl_replace_line(text, clear_undo)</code></strong> – Replaces the current line buffer (used in custom prompt behavior).</li>
<li data-start="699" data-end="785"><strong data-start="701" data-end="721"><code data-start="703" data-end="719">rl_redisplay()</code></strong> – Redraws the prompt line (useful after signals like <code data-start="775" data-end="783">SIGINT</code>).</li>
</ul>
<hr data-start="787" data-end="790">
<h2 data-start="792" data-end="816">🟢 Output / Debugging</h2>
<p data-start="817" data-end="851">Used to display output and errors.</p>
<ul data-start="853" data-end="1177">
<li data-start="853" data-end="906"><strong data-start="855" data-end="869"><code data-start="857" data-end="867">printf()</code></strong> – Prints formatted output to stdout.</li>
<li data-start="907" data-end="1018"><strong data-start="909" data-end="935"><code data-start="911" data-end="933">write(fd, buf, size)</code></strong> – Low-level function to write to a file descriptor (used for redirections, pipes).</li>
<li data-start="1019" data-end="1095"><strong data-start="1021" data-end="1042"><code data-start="1023" data-end="1040">strerror(errno)</code></strong> – Converts an error code to a human-readable string.</li>
<li data-start="1096" data-end="1177"><strong data-start="1098" data-end="1115"><code data-start="1100" data-end="1113">perror(msg)</code></strong> – Prints a descriptive error message to stderr using <code data-start="1169" data-end="1176">errno</code>.</li>
</ul>
<hr data-start="1179" data-end="1182">
<h2 data-start="1184" data-end="1207">🟢 Memory Management</h2>
<p data-start="1208" data-end="1224">Used everywhere.</p>
<ul data-start="1226" data-end="1299">
<li data-start="1226" data-end="1299"><strong data-start="1228" data-end="1253"><code data-start="1230" data-end="1240">malloc()</code> / <code data-start="1243" data-end="1251">free()</code></strong> – Dynamic memory allocation and deallocation.</li>
</ul>
<hr data-start="1301" data-end="1304">
<h2 data-start="1306" data-end="1356">🟢 File and I/O Management (Redirections, Exec)</h2>
<p data-start="1357" data-end="1416">These are essential for redirection and launching commands.</p>
<ul data-start="1418" data-end="1817">
<li data-start="1418" data-end="1513"><strong data-start="1420" data-end="1444"><code data-start="1422" data-end="1442">access(path, mode)</code></strong> – Checks if a file exists or is executable (used before <code data-start="1501" data-end="1511">execve()</code>).</li>
<li data-start="1514" data-end="1592"><strong data-start="1516" data-end="1563"><code data-start="1518" data-end="1526">open()</code> / <code data-start="1529" data-end="1537">read()</code> / <code data-start="1540" data-end="1549">write()</code> / <code data-start="1552" data-end="1561">close()</code></strong> – Low-level file operations.</li>
<li data-start="1593" data-end="1664"><strong data-start="1595" data-end="1609"><code data-start="1597" data-end="1607">unlink()</code></strong> – Deletes a file (useful for temporary heredoc files).</li>
<li data-start="1665" data-end="1755"><strong data-start="1667" data-end="1689"><code data-start="1669" data-end="1676">dup()</code> / <code data-start="1679" data-end="1687">dup2()</code></strong> – Duplicates file descriptors (used for redirections and piping).</li>
<li data-start="1756" data-end="1817"><strong data-start="1758" data-end="1775"><code data-start="1760" data-end="1773">pipe(fd[2])</code></strong> – Creates a pipe (used in <code data-start="1802" data-end="1815">cmd1 | cmd2</code>).</li>
</ul>
<hr data-start="1819" data-end="1822">
<h2 data-start="1824" data-end="1849">🟢 Process &amp; Execution</h2>
<p data-start="1850" data-end="1892">Used to create and manage child processes.</p>
<ul data-start="1894" data-end="2137">
<li data-start="1894" data-end="1939"><strong data-start="1896" data-end="1908"><code data-start="1898" data-end="1906">fork()</code></strong> – Creates a new child process.</li>
<li data-start="1940" data-end="2045"><strong data-start="1942" data-end="1972"><code data-start="1944" data-end="1970">execve(path, argv, envp)</code></strong> – Replaces the current process image with a new one (executes commands).</li>
<li data-start="2046" data-end="2137"><strong data-start="2048" data-end="2098"><code data-start="2050" data-end="2058">wait()</code> / <code data-start="2061" data-end="2072">waitpid()</code> / <code data-start="2075" data-end="2084">wait3()</code> / <code data-start="2087" data-end="2096">wait4()</code></strong> – Waits for child processes to finish.</li>
</ul>
<hr data-start="2139" data-end="2142">
<h2 data-start="2144" data-end="2157">🟢 Signals</h2>
<p data-start="2158" data-end="2197">Used to handle Ctrl+C, Ctrl+\ and more.</p>
<ul data-start="2199" data-end="2530">
<li data-start="2199" data-end="2252"><strong data-start="2201" data-end="2227"><code data-start="2203" data-end="2225">signal(sig, handler)</code></strong> – Sets a signal handler.</li>
<li data-start="2253" data-end="2363"><strong data-start="2255" data-end="2306"><code data-start="2257" data-end="2270">sigaction()</code> / <code data-start="2273" data-end="2288">sigemptyset()</code> / <code data-start="2291" data-end="2304">sigaddset()</code></strong> – Fine-grained signal handling (better than <code data-start="2351" data-end="2361">signal()</code>).</li>
<li data-start="2364" data-end="2445"><strong data-start="2366" data-end="2386"><code data-start="2368" data-end="2384">kill(pid, sig)</code></strong> – Sends a signal to a process (e.g. terminating children).</li>
<li data-start="2446" data-end="2530"><strong data-start="2448" data-end="2460"><code data-start="2450" data-end="2458">exit()</code></strong> – Exits the current process (used for built-in <code data-start="2508" data-end="2514">exit</code>, errors, etc.).</li>
</ul>
<hr data-start="2532" data-end="2535">
<h2 data-start="2537" data-end="2562">🟢 Environment &amp; Paths</h2>
<p data-start="2563" data-end="2619">Needed for <code data-start="2574" data-end="2580">$VAR</code>, <code data-start="2582" data-end="2587">env</code>, <code data-start="2589" data-end="2593">cd</code>, and running executables.</p>
<ul data-start="2621" data-end="2822">
<li data-start="2621" data-end="2686"><strong data-start="2623" data-end="2641"><code data-start="2625" data-end="2639">getenv(name)</code></strong> – Gets the value of an environment variable.</li>
<li data-start="2687" data-end="2766"><strong data-start="2689" data-end="2712"><code data-start="2691" data-end="2710">getcwd(buf, size)</code></strong> – Gets the current working directory (<code data-start="2751" data-end="2756">pwd</code> builtin).</li>
<li data-start="2767" data-end="2822"><strong data-start="2769" data-end="2786"><code data-start="2771" data-end="2784">chdir(path)</code></strong> – Changes directory (<code data-start="2808" data-end="2812">cd</code> builtin).</li>
</ul>
<hr data-start="2824" data-end="2827">
<h2 data-start="2829" data-end="2851">🟢 File Information</h2>
<p data-start="2852" data-end="2893">Used to check file types and permissions.</p>
<ul data-start="2895" data-end="3024">
<li data-start="2895" data-end="3024"><strong data-start="2897" data-end="2933"><code data-start="2899" data-end="2907">stat()</code> / <code data-start="2910" data-end="2919">lstat()</code> / <code data-start="2922" data-end="2931">fstat()</code></strong> – Retrieves metadata about files (may be helpful for better <code data-start="2994" data-end="3000">exec</code> handling or built-ins).</li>
</ul>
<hr data-start="3026" data-end="3029">
<h2 data-start="3031" data-end="3055">🟢 Directory Handling</h2>
<p data-start="3056" data-end="3129">Used for <code data-start="3065" data-end="3070">env</code>, <code data-start="3072" data-end="3080">export</code>, <code data-start="3082" data-end="3089">unset</code>, and wildcard expansion (<code data-start="3115" data-end="3118">*</code> in bonus).</p>
<ul data-start="3131" data-end="3262">
<li data-start="3131" data-end="3262"><strong data-start="3133" data-end="3177"><code data-start="3135" data-end="3146">opendir()</code> / <code data-start="3149" data-end="3160">readdir()</code> / <code data-start="3163" data-end="3175">closedir()</code></strong> – Opens, reads, and closes directories (for <code data-start="3222" data-end="3225">*</code> wildcard in bonus or <code data-start="3247" data-end="3252">env</code> listing).</li>
</ul>
<hr data-start="3264" data-end="3267">
<h2 data-start="3269" data-end="3292">🟢 Terminal Handling</h2>
<p data-start="3293" data-end="3395">Used for interactive prompt behavior, e.g., disabling echo in heredoc, managing input/output behavior.</p>
<ul data-start="3397" data-end="3685">
<li data-start="3397" data-end="3509"><strong data-start="3399" data-end="3415"><code data-start="3401" data-end="3413">isatty(fd)</code></strong> – Checks if the file descriptor is a terminal (helps distinguish interactive vs script mode).</li>
<li data-start="3510" data-end="3598"><strong data-start="3512" data-end="3543"><code data-start="3514" data-end="3527">ttyname(fd)</code> / <code data-start="3530" data-end="3541">ttyslot()</code></strong> – Gets terminal device name (rarely used but allowed).</li>
<li data-start="3599" data-end="3685"><strong data-start="3601" data-end="3614"><code data-start="3603" data-end="3612">ioctl()</code></strong> – Performs terminal-specific control (optional; might help for bonus).</li>
</ul>
<hr data-start="3687" data-end="3690">
<h2 data-start="3692" data-end="3750">🟢 Terminal Capabilities (Bonus Prompt, Cursor Control)</h2>
<p data-start="3751" data-end="3801">Used for custom terminal output or better prompts.</p>
<ul data-start="3803" data-end="4067">
<li data-start="3803" data-end="3865"><strong data-start="3805" data-end="3833"><code data-start="3807" data-end="3831">tgetent(buf, termname)</code></strong> – Initializes termcap database.</li>
<li data-start="3866" data-end="3954"><strong data-start="3868" data-end="3919"><code data-start="3870" data-end="3889">tgetflag(capname)</code> / <code data-start="3892" data-end="3903">tgetnum()</code> / <code data-start="3906" data-end="3917">tgetstr()</code></strong> – Retrieves terminal capabilities.</li>
<li data-start="3955" data-end="4067"><strong data-start="3957" data-end="3982"><code data-start="3959" data-end="3968">tgoto()</code> / <code data-start="3971" data-end="3980">tputs()</code></strong> – Moves cursor, applies capabilities (used for fancy prompts, clearing lines, etc.).</li>
</ul>
<hr data-start="4069" data-end="4072">

<h2 data-start="4074" data-end="4107">🟠 Summary Table (Cheat Sheet)</h2>
<div class="overflow-x-auto contain-inline-size">

Category | Examples
-- | --
Input | readline, add_history
Output | printf, write, perror, strerror
Memory | malloc, free
Files/IO | open, close, dup2, pipe, access, unlink
Processes | fork, execve, wait, waitpid
Signals | signal, sigaction, kill
Env/Paths | getenv, chdir, getcwd
File Info | stat, fstat, lstat
Directories | opendir, readdir, closedir
Terminal I/O | isatty, tcgetattr, tcsetattr, ioctl
Termcap | tgetent, tputs, tgoto, tgetstr

</div>
<hr data-start="5037" data-end="5040">
