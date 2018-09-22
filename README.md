<h1>UCM Fund Tracker</h1>

<p>UCM Fund Tracker fetches recent transactions and pushes a phone nofication of such if available. This was made because there is no official way to do so yet for some bad reason. (only for use by students in University of California, Merced)</p>

<h2>Building</h2>

<p>First, you will need <a href="https://www.pushbullet.com/">Pushbullet</a> on your phone. Register an account and obtain an API key on your settings.
Next, do a typical CMake build (preferably in a subdirectory).</p>

<blockquote>
  <p>mkdir build
cd build
cmake ..
make</p>
</blockquote>

<p>This produces an executable called <code>main</code>. You're free to run it by the following:</p>

<blockquote>
  <p>./main -u USERNAME -p PASSWORD -e PUSHBULLET EMAIL -a PUSHBULLET API KEY</p>
</blockquote>

<h2>Dependencies</h2>

<p><a href="https://github.com/whoshuu/cpr">cpr</a>
myhtml
pthread</p>
