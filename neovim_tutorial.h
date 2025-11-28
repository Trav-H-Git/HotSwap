// ┌────────────────────────────────────────────────────────────────────────────┐
// │                    🧠 Neovim + C++ Tutorial File (Enhanced)               │
// │   Tailored for VS Code with Neovim, using semicolon (;) as leader key     │
// └────────────────────────────────────────────────────────────────────────────┘

#include <iostream>
#include <vector>
#include <string>

int main() {
    // 1️⃣ NAVIGATION BASICS (Normal Mode)
    // Move cursor: h (left), j (down), k (up), l (right)
    // w: next word start, b: previous word start, e: end of curdfurent/next word
    // 0: jump to start of line, $: jump to end of line
    // ^: jump to first non-blank character of line
    std::cout << "Hello, Neovim in VS Code!" << std::endl;

    // gg: jump to top of file
    // G: jump to bottom of file
    // {n}G: jump to line n (e.g., 10G goes to line 10)
    // :{n} : jump to line n (e.g., :10<Enter> goes to line 10)
    // +{n} / -{n}: relative line jumps (e.g., +3 moves 3 lines down, -2 moves 2 lines up)

    // 2️⃣ EDITING & INSERT MODE
    // Enter insert mode: i (before cursor), I (line start), a (after cursor), A (line end)
    // o (new line below), O (new line above)
    // Escape insert mode: jk (configured in many setups) or <Esc>
    // u: undo; <Ctrl>-r: redo
    // dd: delete line; yy: yank (copy) line; p: paste after cursor; P: paste before
    // cc: change (delete and insert) line; cw: change word; ciw: change inner word
    std::vector<int> nums = {1, 2, 3, 4};
    nums.push_back(5); // Try ciw to change 'push_back' to 'emplace_back'

    // 3️⃣SEARCH & REPLACE
    // /pattern: search forward (e.g., /vector); ?pattern: search backward
    // n: next match; N: previous match
    // :%s/old/new/g: replace all occurrences (e.g., :%s/push_back/emplace_back/g)
    // :%s/old/new/gc: replace with confirmation
    // * : search for word under cursor forward; # : search backward
    std::string message = "Neovim is awesome!";
    if (!message.empty()) {
        std::cout << message << std::endl;
    }

    // 4️⃣ SPLITS & TABS
    // :split or :sp: horizontal split; :vsplit or :vs: vertical split
    // ;w h/j/k/l: move between splits (maps to <Ctrl>-w h/j/k/l in VS Code)
    // :tabnew: open new tab; gt: next tab; gT: previous tab
    // ;t: open new tab in VS Code (workbench.action.files.newUntitledFile)
    //vim.keymap.set('n', ';t', vc('workbench.action.files.newUntitledFile'), { silent = true });

    // 5️⃣ FILE & BUFFER MANAGEMENT (Customized for ; Leader)
    // ;e: open file explorer (workbench.view.explorer)
    // ;p: quick open file (workbench.action.quickOpen)
    // ;P: show active file in explorer (workbench.files.action.showActiveFileInExplorer)
    // :e filename.cpp: edit another file
    // :ls: list open buffers; :b {n}: switch to buffer number n
    // :bd: close current buffer; :bw: wipe buffer (removes from memory)
    // Example: Open another file and split
    // :vs other_file.cpp
    for (int x : nums) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    // 6️⃣ BLOCK OPERATIONS & INDENTATION
    // Visual mode: v (character), V (line), <Ctrl>-v (block)
    // In visual mode: d (delete), y (yank), c (change)
    // > / < : indent/outdent selected lines
    // ==: auto-indent current line; =a{: indent current block
    // gg=G: indent entire file (great for C++ code)
    // Ensure cindent is set: vim.opt.cindent = true
    //vim.opt.cindent = true; // Add to init.lua for C++ auto-indentation
    int indent_example = 0;
    if (indent_example > 0) {
        std::cout << "Indented block!" << std::endl;
    }

    // 7️⃣ ADVANCED MOVEMENTS
    // %: jump between matching braces/parens (e.g., {}, (), [])
    // {/}: jump to previous/next paragraph or code block
    // Ctrl-u / Ctrl-d: scroll half-page up/down
    // Ctrl-b / Ctrl-f: scroll full page up/down
    // zz: center cursor line; zt: move cursor line to top; zb: to bottom
    // f{char} / F{char}: jump to next/previous char on line
    // t{char} / T{char}: jump to before next/previous char
    std::vector<std::string> words = {"Neovim", "VS", "Code"};
    for (const auto& word : words) {
        std::cout << word << " "; // Try f; to jump to semicolons
    }
    std::cout << std::endl;

    // 8️⃣ vs code integration with neovim
    // your init.lua uses ; as leader for vs code commands
    // add to init.lua for more mappings:
    // vim.keymap.set('n', ';f', vc('editor.action.formatdocument'), { silent = true }) // format code
    // vim.keymap.set('n', ';g', vc('workbench.action.gotosymbol'), { silent = true }) // go to symbol
    // use :w to save (maps to vs code save)
    // ;e to toggle file explorer, ;p to search files, ;p to locate current file
    int sum = 0;
    for (int v : nums) {
        sum += v; // try ;f to format this block
    }
    std::cout << "sum: " << sum << std::endl;
    
    // q{a-z} : record macro to register a-z; @{a-z} : replay macro
    // :set number : show line numbers; :set relativenumber : relative line numbers
    // :marks : list all marks; m{a-z} : set mark; `{a-z} : jump to mark
    // Add to init.lua: vim.opt.number = true; vim.opt.relativenumber = true
    //vim.opt.number = true;
    //vim.opt.relativenumber = true;

    // ✍️ PRACTICE EXERCISE
    // 1. Jump to line 10 (:10 or 10G)
    // 2. Change 'push_back' to 'emplace_back' using :%s
    // 3. Use ;e to open explorer, ;p to find a file
    // 4. Try f; to jump to semicolons, 0/$ to start/end of line
    // 5. Indent the for loop below with > in visual mode
    for (int i = 0; i < 5; i++) {
                std::cout << "Practice: " << i << std::endl;
    }

    return 0;
}
