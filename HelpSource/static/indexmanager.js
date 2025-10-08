// autoNumbering.js - Automatically numbers tables, figures, code snippets, post windows, and formulas
// and replaces link texts with corresponding figure/table numbers

document.addEventListener('DOMContentLoaded', function() {
    // Track counters for each numbering type
    const counters = {
        'Table': {},
        'Figure': {},
        'Code Snippet': {},
        'Post Window': {},
        'Formula': {},
        'Score': {}
    };
    
    // Process all tables for automatic numbering
    const tables = document.querySelectorAll('table');
    
    tables.forEach(table => {
        // Process all cells in the table for different caption types
        const cells = table.querySelectorAll('th, td');
        
        cells.forEach(cell => {
            // Process all caption types
            const types = ['Table', 'Figure', 'Code Snippet', 'Post Window', 'Formula', 'Score'];
            types.forEach(type => {
                processCaption(cell, type, counters);
            });
        });
    });
    
    // Function to process caption elements in tables
    function processCaption(element, type, counters) {
        const strongElements = element.querySelectorAll('strong');
        
        strongElements.forEach(strongElement => {
            const text = strongElement.textContent;
            
            // Match various patterns:
            // - "Figure." (no numbers)
            // - "Figure 1." (single number)
            // - "Figure 1.1." (multiple numbers)
            // - "Figure 1.1.1." etc.
            const pattern = new RegExp(`^${type}(?:\\s+(\\d+(?:\\.\\d+)*))?\\.$`);
            const match = text.match(pattern);
            
            if (match) {
                let numberString = match[1] || ''; // Get the number part if exists
                
                if (numberString === '') {
                    // Case: "Figure." - treat as "Figure 0."
                    numberString = '0';
                }
                
                // Initialize or increment counter for this number string
                if (!counters[type][numberString]) {
                    counters[type][numberString] = 0;
                }
                counters[type][numberString]++;
                
                // Create the new numbering
                let newNumberString;
                if (numberString === '0') {
                    // For "Figure." case, just use the counter
                    newNumberString = `${counters[type][numberString]}`;
                } else {
                    // For numbered cases, append the counter
                    newNumberString = `${numberString}.${counters[type][numberString]}`;
                }
                
                // Update the text with new numbering and add space after the last dot
                const newText = `${type} ${newNumberString}. `;
                strongElement.textContent = newText;
                
                // Add visual feedback
                element.classList.add('highlight');
                
                console.log(`Updated ${type}: ${text} -> ${newText}`);
            }
        });
    }
    
    // Log counters for debugging
    console.log('Final numbering counters:', counters);
    
    // Now replace link texts with corresponding figure/table numbers
    replaceLinkTexts();
    
    function replaceLinkTexts() {
        // Find all links with href starting with #
        const links = document.querySelectorAll('a[href^="#"]');
        
        links.forEach(link => {
            const href = link.getAttribute('href');
            const anchorName = href.substring(1); // Remove the # symbol
            
            // Find the anchor element with the matching name
            const anchor = document.querySelector(`a.anchor[name="${anchorName}"]`);
            
            if (anchor) {
                // Find the strong element that precedes the anchor
                let previousElement = anchor.previousElementSibling;
                let strongElement = null;
                
                // Traverse previous siblings to find the closest strong element
                while (previousElement) {
                    if (previousElement.tagName === 'STRONG') {
                        strongElement = previousElement;
                        break;
                    }
                    previousElement = previousElement.previousElementSibling;
                }
                
                // If we found a strong element, replace the link text
                if (strongElement) {
                    const strongText = strongElement.textContent.trim();
                    
                    // Extract just the numbering part (e.g., "Figure 2." from "Figure 2. Client/Server architecture")
                    const match = strongText.match(/^(Table|Figure|Code Snippet|Post Window|Formula|Score)\s+[\d.]+\./);
                    
                    if (match) {
                        // Remove the trailing dot from the matched text
                        let linkText = match[0].trim();
                        if (linkText.endsWith('.')) {
                            linkText = linkText.slice(0, -1);
                        }
                        link.textContent = linkText;
                        console.log(`Replaced link text: ${link.textContent}`);
                    }
                }
            }
        });
    }
});
