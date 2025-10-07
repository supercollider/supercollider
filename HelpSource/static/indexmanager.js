// autoNumbering.js - Automatically numbers tables, figures, code snippets, post windows, and formulas

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
    
    // Process all tables
    const tables = document.querySelectorAll('table');
    
    tables.forEach(table => {
        // Process all cells in the table for different caption types
        const cells = table.querySelectorAll('th, td');
        
        cells.forEach(cell => {
            // Process table captions (any cell)
            processCaption(cell, 'Table', counters);
            
            // Process figure captions (any cell)
            processCaption(cell, 'Figure', counters);
            
            // Process code snippet captions (any cell)
            processCaption(cell, 'Code Snippet', counters);
            
            // Process post window captions (any cell)
            processCaption(cell, 'Post Window', counters);
            
            // Process formula captions (any cell)
            processCaption(cell, 'Formula', counters);
            
            // Process formula captions (any cell)
            processCaption(cell, 'Score', counters);
        });
    });
    
    // Function to process caption elements in tables
    function processCaption(element, type, counters) {
        const strongElements = element.querySelectorAll('strong');
        
        strongElements.forEach(strongElement => {
            const text = strongElement.textContent;
            // Match pattern like "Table 1." or "Figure 2."
            const match = text.match(new RegExp(`^${type}\\s+(\\d+)\\.`));
            
            if (match) {
                const mainNumber = match[1];
                
                // Initialize or increment counter for this main number
                if (!counters[type][mainNumber]) {
                    counters[type][mainNumber] = 0;
                }
                counters[type][mainNumber]++;
                
                // Update the text with new numbering and add space after the last dot
                const newText = text.replace(
                    new RegExp(`^${type}\\s+\\d+\\.`), 
                    `${type} ${mainNumber}.${counters[type][mainNumber]}. `
                );
                strongElement.textContent = newText;
                
                // Add visual feedback
                element.classList.add('highlight');
                
                console.log(`Updated ${type}: ${text} -> ${newText}`);
            }
        });
    }
    
    // Log counters for debugging
    console.log('Final numbering counters:', counters);
});
