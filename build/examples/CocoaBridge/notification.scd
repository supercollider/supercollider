
// blackrain
c = SCNSObject("NSNotificationCenter", "defaultCenter");
o = SCNSObject("NSObject", "init", []);

(
// SCNSObject.registerNotification(notificationName, func, obj=this);
o.registerNotification('MyNotification', { arg notificationName, nsNotification, nsObj;
	var notification;
	 notification = nsNotification.asNSReturn;
	[notificationName, notification.className, nsObj.asNSReturn.invoke("hash")].postln;
}, nil);
)

o.invoke("hash");
c.invoke("postNotificationName:object:", ["MyNotification", o], true);

d = SCNSObject("NSObject", "init", []);
d.invoke("hash");
c.invoke("postNotificationName:object:", ["MyNotification", d], true);

[o, c, d].do(_.release);

SCNSObject.dumpPool;

SCNSObject.freePool;