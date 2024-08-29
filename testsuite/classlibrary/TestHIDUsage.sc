TestHIDUsage : UnitTest {
	classvar <usagePages;
	*initClass {
		usagePages = (Platform.resourceDir ++ "/HID_Support/hut/HidUsageTables.json").parseJSONFile["UsagePages"].select(
			{|page|
				((1..16) ++ [20, 64]).includes(page["Id"].asInteger) // TODO open this up to the full list of usages, including generated
			}
		);
		this.reportPasses=false;
		^super.initClass();
	}
	formatName {|name|
		^name.asString.replace($ , "").asSymbol;
	}

	test_getUsageDescription_returnsPageNameAndUsageName {
		^usagePages.do({|page|
			page["UsageIds"].do({|usage|
				var expected = [this.formatName(page["Name"]), this.formatName(usage["Name"])];
				var result = HIDUsage.getUsageDescription(page["Id"].asInteger, usage["Id"].asInteger);
				this.assertEquals(result, expected,
					"HIDUsage.getUsageDescription(%, %) should return %; got %".format(
						page["Id"], usage["Id"], expected, result
					));
			});
		});
	}
	test_idsToName_GetsUsageName {
		^usagePages.do({|page|
			page["UsageIds"].do({|usage|
				var expected = this.formatName(usage["Name"]);
				var result = HIDUsage.idsToName(page["Id"].asInteger, usage["Id"].asInteger);
				this.assertEquals(
					result,
					expected,
					"HIDUsage.idsToName(%, %) should return usage name %; got %".format(
						page["Id"], usage["Id"], expected, result
					);
				);
			});
		});
	}
	test_getUsageIds_GetsUsage_Ids {
		^usagePages.do({|page|
			page["UsageIds"].do({|usage|
				var usageName = this.formatName(usage["Name"]);
				var expected = [page["Id"].asInteger, usage["Id"].asInteger];
				var result = HIDUsage.getUsageIds(usageName);
				this.assertEquals(
					result,
					expected,
					"HIDUsage.getUsageIds(%) on page % should return %; got % (from page %)".format(
						usageName.cs,
						this.formatName(page["Name"]),
						expected.cs,
						result.cs,
						HIDUsage.performList(\getUsageDescription, result)[0]
					)
				)
			});
		})
	}
}
