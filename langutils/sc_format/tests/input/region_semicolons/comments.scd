// Comments between top-level items. The `;` goes after the first item, before the comments.
x = 1 // c
y = 2
x = 1 /* c */
y = 2
x = 1 /* a */ /* b */
y = 2
x = 1
// on its own line
y = 2
x = 1
/* a */
// b
y = 2
(
a = 1 // in the region
)
(
a = 1; // in the region, after a `;`
)
(
b = 2
) /* after it */
( /* in the next */
c = 3
)
(
d = 4
)
// before the next
( // after its `(`
e = 5
)
// A comment that holds a `;` isn't one.
x = 1 // ;
y = 2
x = 1 /* ; */
y = 2
// Already a `;`, after a comment.
x = 1 /* c */;
y = 2
x = 1 // c
;
y = 2
z = 3 // the last
